#include <QCoreApplication>
#include <QWindow>
#include <QDebug>
#include <QWebChannel>
#include <QShowEvent>
#include <QPointF>
#include <QApplication>
#include <QX11Info>
#include <QJsonObject>

#include "include/cef_task.h"

#include "browser_handler.h"
#include "webview.h"
#include "webchannel_transport.h"


#include <gdk/gdkx.h>
const QString WebView::kUrlBlank = "about:blank";

QHash<int64, WebView*> WebView::__cache__;

WebView* WebView::find(int64 browser_id) {
    return WebView::__cache__[browser_id];
}

WebView::WebView(QWidget* parent)
    : QWidget(parent),
      m_browser_widget(0),
      m_channel(new QWebChannel(this)) {

    qDebug() << __FUNCTION__  << this;
    setAttribute(Qt::WA_NativeWindow);
    setAttribute(Qt::WA_DontCreateNativeAncestors);
}

WebView::~WebView() {
    Q_FOREACH(auto id, m_transports.keys()) {
        this->pageDestroyed(id);
    }
    delete m_channel;

    if (m_browser) {
        WebView::__cache__.remove(m_browser->GetIdentifier());
        qDebug() << "Remove webview m_browser''''''";

        auto host = m_browser->GetHost();
        host->ParentWindowWillClose();
        host->CloseBrowser(true);
    }

    qDebug() << __FUNCTION__;
}

void WebView::setHtml(const QString& html, const QUrl& baseUrl)  {
    qDebug() << "Not Implement setHtml";
}

void WebView::setUrl(const QUrl& url) {
    //qDebug() << __FUNCTION__ << url;
    url_ = url;

    if (!m_browser) {
        CreateBrowser();
    }
}

gboolean convert_gtk_event(GtkWidget* w, GdkEvent* e, QWidget* q)
{
    switch(e->type) {
    case GDK_ENTER_NOTIFY:
    {
        GdkEventCrossing* ec = (GdkEventCrossing*)e;
        QEnterEvent eq(QPointF(ec->x, ec->y), QPointF(ec->x, ec->y), QPointF(ec->x_root, ec->y_root));
        QApplication::sendEvent(q, &eq);
        break;
    }
    }
    return false;
}

void handle_gtk_widnow(GtkWidget* g, QObject* q) {
    g_signal_connect(g, "event-after", (GCallback)convert_gtk_event, q);
}

void WebView::registerObject(const QString& name, QObject* obj) {
    m_channel->registerObject(name, obj);
}

void WebView::showEvent(QShowEvent* e) {
    if (m_browser_widget) {
        gtk_widget_show_all(m_browser_widget);
    }
    QWidget::showEvent(e);
}
void WebView::closeEvent(QCloseEvent* ev) {
    qDebug() << "Close......" << this;
    QWidget::closeEvent(ev);
}
void WebView::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);

    if (m_browser_widget) {
        QSize size = event->size();

        XWindowChanges changes;
        changes.x = 0;
        changes.y = 0;
        changes.width = size.width();
        changes.height = size.height();

        XConfigureWindow(QX11Info::display(),
                         GDK_WINDOW_XID(gtk_widget_get_window(m_browser_widget)),
                         CWX | CWY | CWHeight | CWWidth, &changes);

        GtkAllocation rect{0,0, size.width(), size.height()};
        gtk_widget_size_allocate(m_browser_widget, &rect);
    }
}


bool WebView::CreateBrowser() {
    Q_ASSERT(CefCurrentlyOn(TID_UI));
    mutex_.lock();
    QSize size(800, 800);
    if (m_browser) {
        mutex_.unlock();
        return true;
    }

    qDebug() << __FUNCTION__ << __LINE__ << getpid();

    CefRect rect(0, 0, size.width(), size.height());

    CefWindowInfo info;
    CefBrowserSettings settings;

    m_browser_widget = gtk_window_new(GTK_WINDOW_POPUP);
    handle_gtk_widnow(m_browser_widget, this);

    gtk_widget_realize(m_browser_widget);

    // By default, as a child window.
    info.SetAsChild(m_browser_widget);

    QString url = url_.isEmpty() ? kUrlBlank : url_.toString();
    m_browser = CefBrowserHost::CreateBrowserSync(info,
                                                  BrowserHandler::GetInstance(),
                                                  CefString(url.toStdWString()),
                                                  settings,
                                                  NULL);

    WebView::__cache__[m_browser->GetIdentifier()] = this;

    XID xid = GDK_WINDOW_XID(gtk_widget_get_window(m_browser_widget));


    QWindow::fromWinId(xid)->setParent(this->windowHandle());


    mutex_.unlock();
    return true;
}

void WebView::pageCreated(const int64 id) {
    Q_ASSERT(CefCurrentlyOn(TID_UI));
    CefWebChannelTransport* transport = new CefWebChannelTransport(m_browser, id, this);
    m_transports[id] = transport;
    m_channel->connectTo(transport);

    qDebug() << "connectTo" << transport << id;
}

void WebView::pageDestroyed(const int64 id) {
    Q_ASSERT(CefCurrentlyOn(TID_UI));
    CefWebChannelTransport* transport = m_transports.take(id);
    if (!transport) {
        qWarning() << "pageDestroyed without a valid transport" << m_transports << id;
        return;
    }
    qDebug() << "disconnectFrom" << transport << id;
    m_channel->disconnectFrom(transport);
    delete transport;
}

void WebView::pageTransportMessageReceived(const int64 id, QJsonObject data) {
    Q_ASSERT(CefCurrentlyOn(TID_UI));
    CefWebChannelTransport* transport = m_transports[id];
    if (!transport) {
        qWarning() << "pageTransportMessageReceived without a valid transport" << m_transports << id;
        return;
    }
//    qDebug() << "messageReceived" << transport << id << data;
    Q_EMIT transport->messageReceived(data, transport);
}
