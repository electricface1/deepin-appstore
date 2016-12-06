#ifndef __RENDERER_HANNDLER_H__
#define __RENDERER_HANNDLER_H__

#include "include/cef_app.h"

class RendererHandler : public CefApp,
                  public CefRenderProcessHandler {
 public:
  RendererHandler();

  virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() OVERRIDE {
      return this;
  }

  virtual void OnContextCreated(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                CefRefPtr<CefV8Context> context) OVERRIDE;
  
  virtual void OnContextReleased(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                CefRefPtr<CefV8Context> context) OVERRIDE;
    
  virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                        CefProcessId source_process,
                                        CefRefPtr<CefProcessMessage> message) OVERRIDE;

 private:
  // Include the default reference counting implementation.
  IMPLEMENT_REFCOUNTING(RendererHandler);
};

#endif
