#ifndef __WEBCHANNEL_TRANSPORT_H__
#define __WEBCHANNEL_TRANSPORT_H__

#include <QWebChannelAbstractTransport>

#include "include/cef_frame.h"

#define MAKE_INT64(int_low, int_high)                                   \
    ((int64) (((int) (int_low)) | ((int64) ((int) (int_high))) << 32))
#define LOW_INT(int64_val) ((int) (int64_val))
#define HIGH_INT(int64_val) ((int) (((int64) (int64_val) >> 32) & 0xFFFFFFFFL))

#define ChannelPageCreatedMessage "qwebchannel_page_created"
#define ChannelPageDestroyedMessage "qwebchannel_page_destroyed"
#define ChannelPageOnMsgMessage "qwebchannel_onmessage"

class QJsonObject;
    
class CefWebChannelTransport : public QWebChannelAbstractTransport {
    Q_OBJECT
    
public:
    explicit CefWebChannelTransport(CefRefPtr<CefBrowser> browser, const int64 id, QObject* parent = nullptr);

    virtual ~CefWebChannelTransport();
    
public Q_SLOTS:
    virtual void sendMessage(const QJsonObject& message) Q_DECL_OVERRIDE;
    
private:
    CefRefPtr<CefBrowser> m_browser;
    int64 m_frame_id;
    
    Q_DISABLE_COPY(CefWebChannelTransport)
};

void RegisterWebChannelTransportObject(CefRefPtr<CefFrame> frame);

#endif
