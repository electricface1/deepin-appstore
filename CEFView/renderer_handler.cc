// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
#include <QDebug>
#include <sys/types.h>
#include <unistd.h>
#include "include/cef_browser.h"

#include "renderer_handler.h"
#include "webchannel_transport.h"

RendererHandler::RendererHandler() {
}

void RendererHandler::OnContextCreated(CefRefPtr<CefBrowser> browser,
                                 CefRefPtr<CefFrame> frame,
                                 CefRefPtr<CefV8Context> context) {
    Q_ASSERT(CefCurrentlyOn(TID_RENDERER));
    
    qDebug() << "OnContextCreated..PID" << getpid();

    RegisterWebChannelTransportObject(frame);

    int64 frame_id = frame->GetIdentifier();
    CefRefPtr<CefProcessMessage> msg= CefProcessMessage::Create(ChannelPageCreatedMessage);
    CefRefPtr<CefListValue> args = msg->GetArgumentList();
    args->SetInt(0, LOW_INT(frame_id));
    args->SetInt(1, HIGH_INT(frame_id));
    browser->SendProcessMessage(PID_BROWSER, msg);
}

void RendererHandler::OnContextReleased(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                       CefRefPtr<CefV8Context> context) {
    Q_ASSERT(CefCurrentlyOn(TID_RENDERER));
    
    int64 frame_id = frame->GetIdentifier();
    CefRefPtr<CefProcessMessage> msg= CefProcessMessage::Create(ChannelPageDestroyedMessage);
    CefRefPtr<CefListValue> args = msg->GetArgumentList();
    args->SetInt(0, LOW_INT(frame_id));
    args->SetInt(1, HIGH_INT(frame_id));
    browser->SendProcessMessage(PID_BROWSER, msg);
}

bool RendererHandler::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                         CefProcessId source_process,
                                         CefRefPtr<CefProcessMessage> message) {
    Q_ASSERT(CefCurrentlyOn(TID_RENDERER));
    
    if (message->GetName() == ChannelPageOnMsgMessage) {
        CefRefPtr<CefListValue> args = message->GetArgumentList();
        const int64 frame_id = MAKE_INT64(args->GetInt(0), args->GetInt(1));
        CefRefPtr<CefFrame> frame = browser->GetFrame(frame_id);
        if (!frame) {
            return false;
        }
        
        auto ctx = frame->GetV8Context();
        ctx->Enter();
        
        auto window = ctx->GetGlobal();
        auto transport = window->GetValue("qt")->GetValue("webChannelTransport");
        auto handler = transport->GetValue("onmessage");

        CefV8ValueList vlist;
        CefRefPtr<CefV8Value> resp = CefV8Value::CreateObject(NULL);
        resp->SetValue("data", CefV8Value::CreateString(args->GetString(2)), V8_PROPERTY_ATTRIBUTE_NONE);
        vlist.push_back(resp);
        handler->ExecuteFunction(transport, vlist);
    
        ctx->Exit();

        return true;
    }
    return false;
}
