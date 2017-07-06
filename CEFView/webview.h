#ifndef __WEBVIEW_H__
#define __WEBVIEW_H__
#include <QWidget>
#include <QUrl>
#include <QMutex>
#include <include/cef_browser.h>


class QWebChannel;
class QShowEvent;
class CefWebChannelTransport;

class WebView : public QWidget {
    Q_OBJECT

public:
    static const QString kUrlBlank;
    static WebView* find(int64 browser_id);

    WebView(QWidget* parent = 0);
    virtual ~WebView();

    void setUrl(const QUrl& url);
    void setHtml(const QString& html, const QUrl& baseUrl = QUrl());
    void registerObject(const QString& name, QObject* obj);

Q_SIGNALS:
    //TODO: emit this;
    void titleChanged(const QString& title);

protected:
    virtual void resizeEvent(QResizeEvent*);

public:
    void pageCreated(const int64 id);
    void pageDestroyed(const int64 id);
    void pageTransportMessageReceived(const int64 id, QJsonObject data);

private:
    bool CreateBrowser();
    Q_SLOT void initCEF();
    void syncCEFSize(int);
    QUrl url_;
    QMutex mutex_;

    CefRefPtr<CefBrowser> m_browser;
    QWindow* m_cef_proxy;
    QWebChannel* m_channel;
    QHash<int64, CefWebChannelTransport*> m_transports;

private:
    static QHash<int64, WebView*> __cache__;

    Q_DISABLE_COPY(WebView)
};

#endif
