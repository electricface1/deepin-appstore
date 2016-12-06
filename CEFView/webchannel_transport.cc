#include <QJsonDocument>
#include <QDebug>
#include <QFile>

#include "include/cef_v8.h"

#include "webchannel_transport.h"


CefWebChannelTransport::CefWebChannelTransport(CefRefPtr<CefBrowser> browser, const int64 id, QObject* parent)
    : m_browser(browser), QWebChannelAbstractTransport(parent), m_frame_id(id)
{
    qDebug() << __FUNCTION__ << this << id;
}

CefWebChannelTransport::~CefWebChannelTransport() {
    qDebug() << __FUNCTION__ << this << m_frame_id;
}

class TransportHandler : public CefV8Handler {
public:
    explicit TransportHandler(CefRefPtr<CefBrowser> browser, int64 frame_id)
        : m_browser(browser), m_frame_id(frame_id)
        {}

    virtual bool Execute(const CefString& name,
                         CefRefPtr<CefV8Value> object,
                         const CefV8ValueList& arguments,
                         CefRefPtr<CefV8Value>& retval,
                         CefString& exception) OVERRIDE {
        Q_ASSERT(name == "send");
        Q_ASSERT(CefCurrentlyOn(TID_RENDERER));
        
        CefRefPtr<CefProcessMessage> msg= CefProcessMessage::Create(ChannelPageOnMsgMessage);
            
        CefRefPtr<CefListValue> args = msg->GetArgumentList();
        args->SetInt(0, LOW_INT(m_frame_id));
        args->SetInt(1, HIGH_INT(m_frame_id));
        args->SetString(2, arguments[0]->GetStringValue());

        m_browser->SendProcessMessage(PID_BROWSER, msg);
        return true;
    }
    IMPLEMENT_REFCOUNTING(TransportHandler);


private:
    CefRefPtr<CefBrowser> m_browser;
    int64 m_frame_id;
};

void CefWebChannelTransport::sendMessage(const QJsonObject& message) {
    Q_ASSERT(CefCurrentlyOn(TID_UI));

    if (!m_browser) {
        qWarning() << "browser has been destroyed when send message" << QJsonDocument(message);
        return;
    }


    CefRefPtr<CefProcessMessage> msg= CefProcessMessage::Create(ChannelPageOnMsgMessage);
    CefRefPtr<CefListValue> args = msg->GetArgumentList();
    args->SetInt(0, LOW_INT(m_frame_id));
    args->SetInt(1, HIGH_INT(m_frame_id));

    
    QJsonDocument doc(message);
    args->SetString(2, doc.toJson().toStdString());
//    qDebug() << "Transport::sendMessage............................" << this << m_frame_id << m_browser;
    
    // qDebug() << "Begin sendMessage" << m_browser->GetRefCt();
    // qDebug() << m_browser->GetIdentifier();
    // qDebug() << "End sendMessage";
    
    m_browser->SendProcessMessage(PID_RENDERER, msg);
}



static QString QRC2CefString(const QString& file)
{
    QFile f(file);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString("Failed read " + file);
    }
    return f.readAll();
}

void RegisterWebChannelTransportObject(CefRefPtr<CefFrame> frame) {
    Q_ASSERT(CefCurrentlyOn(TID_RENDERER));
    Q_ASSERT(frame);
    // Require in render process

    auto ctx = frame->GetV8Context();
    if (!ctx) {
        qDebug() << __FUNCTION__ << "can't get V8Context()" << frame->GetIdentifier();
        return;
    }

    ctx->Enter();
    

    CefRefPtr<CefV8Value> js_transport = CefV8Value::CreateObject(NULL);
    CefRefPtr<CefV8Handler> send_handler = new TransportHandler(frame->GetBrowser(), frame->GetIdentifier());
    qDebug() << "INITWebChannelTransport...." << frame->GetIdentifier() << frame->GetBrowser()->GetIdentifier();
    js_transport->SetValue("send", CefV8Value::CreateFunction("send", send_handler), V8_PROPERTY_ATTRIBUTE_NONE);

    CefRefPtr<CefV8Value> qt = CefV8Value::CreateObject(NULL);
    qt->SetValue("webChannelTransport", js_transport, V8_PROPERTY_ATTRIBUTE_NONE);

    ctx->GetGlobal()->SetValue("qt", qt, V8_PROPERTY_ATTRIBUTE_NONE);

    ctx->Exit();

    const CefString code = QRC2CefString(":/qtwebchannel/qwebchannel.js").toStdWString();
    frame->ExecuteJavaScript(code, "/", 0);
}
