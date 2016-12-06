#include <QDebug>
#include <QWebChannel>
#include <QJsonObject>
#include <QJsonDocument>
#include "browser_handler.h"

#include <sstream>
#include <string>

#include "include/cef_app.h"

#include "webview.h"
#include "webchannel_transport.h"

BrowserHandler* BrowserHandler::GetInstance() {
    static BrowserHandler* g_instance = new BrowserHandler;
    return g_instance;
}

bool BrowserHandler::OnProcessMessageReceived( CefRefPtr< CefBrowser > browser, CefProcessId source_process, CefRefPtr< CefProcessMessage > message )
{
    Q_ASSERT(CefCurrentlyOn(TID_UI));

    auto name = QString::fromUtf8(message->GetName().ToString().c_str());
    if (name != ChannelPageOnMsgMessage &&
        name != ChannelPageCreatedMessage &&
        name != ChannelPageDestroyedMessage) {
        qWarning() << "unknonw message" << __FUNCTION__ << name;
        return false;
    }
    
    CefRefPtr<CefListValue> args = message->GetArgumentList();
    const int64 frame_id = MAKE_INT64(args->GetInt(0), args->GetInt(1));
    auto frame = browser->GetFrame(frame_id);
    if (!frame) {
        qWarning() << "Invalid frame_id " << browser->GetIdentifier() << frame_id;
        return false;
    }
    
    WebView* view = WebView::find(browser->GetIdentifier());
    if (!view) {
        return false;
    }
    
    if (name == ChannelPageOnMsgMessage) {
        QString qstr = QString::fromStdWString(args->GetString(2));
        view->pageTransportMessageReceived(frame_id, QJsonDocument::fromJson(qstr.toUtf8()).object());
        return true;
    } else if (name == ChannelPageCreatedMessage) {
        view->pageCreated(frame_id);
        return true;
    } else if (name == ChannelPageDestroyedMessage) {
        view->pageDestroyed(frame_id);
        return true;
    }
    Q_ASSERT(false);
}
