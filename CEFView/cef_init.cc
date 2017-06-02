// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
#include <QApplication>
#include <QTimer>
#include <QWindow>
#include <QMainWindow>
#include <QDebug>
#include <QStandardPaths>

#include <unistd.h>
#include <glib.h>

#include "webview.h"
#include "renderer_handler.h"
#include "browser_handler.h"

#include <X11/Xlib.h>

namespace {
    int XErrorHandlerImpl(Display *display, XErrorEvent *event) {
        qDebug()
            << "X error received: "
            << "type " << event->type << ", "
            << "serial " << event->serial << ", "
            << "error_code " << static_cast<int>(event->error_code) << ", "
            << "request_code " << static_cast<int>(event->request_code) << ", "
            << "minor_code " << static_cast<int>(event->minor_code);
        return 0;
    }

    int XIOErrorHandlerImpl(Display *display) {
        return 0;
    }

}  // namespace

CefSettings BuildCefSettings() {
    // Specify CEF global settings here.
    CefSettings settings;
    // Store cache data will on disk.
    QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/deepin-appstore" ;
    CefString(&settings.cache_path) = cacheDir.toStdWString();

    // Completely disable logging.
    // settings.log_severity = LOGSEVERITY_DISABLE;
    // settings.log_severity = LOGSEVERITY_VERBOSE;

    CefString(&settings.resources_dir_path) = CefString(CEF_ROOT"/Resources");
    CefString(&settings.locales_dir_path) = CefString(CEF_ROOT"/Resources/locales");

    // NOTE: I don't why the default locale value isn't according system locale that it should be by document. So we set it directly.
    char* s = 0;
    if ((s = getenv("LANGUAGE"))  || (s = getenv("LC_ALL")) || (s = getenv("LANG"))) {
        CefString(&settings.locale) = CefString(s);
    }

    auto ua = QApplication::instance()->applicationName() + "/" + QApplication::instance()->applicationVersion();
    if (ua != "/") {
        CefString(&settings.user_agent) = CefString(ua.toStdWString());
    } else {
        qWarning() << "Can't find application version information";
    }

    // settings.remote_debugging_port = 8088;
    return settings;
}

gboolean handle_qt_event(void) {
    // printf("handle_qt_event\n");
    QApplication::instance()->processEvents();
//   qDebug() << "HandleQtMessage" << QApplication::instance()->thread();

   // printf("end_handle_qt_event\n");
    return G_SOURCE_CONTINUE;
}

// Entry point function for all processes.
int InitCEF(int argc, char* argv[]) {
    XSetErrorHandler(XErrorHandlerImpl);
    XSetIOErrorHandler(XIOErrorHandlerImpl);

    // Provide CEF with command-line arguments.
    CefMainArgs main_args(argc, argv);
    CefRefPtr<RendererHandler> app(new RendererHandler);

    // CEF applications have multiple sub-processes (render, plugin, GPU, etc)
    // that share the same executable. This function checks the command-line and,
    // if this is a sub-process, executes the appropriate logic.
    int exit_code = CefExecuteProcess(main_args, app.get(), 0);
    if (exit_code >= 0) {
        // The sub-process has completed so return here.
        return exit_code;
    }

    // Initialize CEF for the browser process.
    if (!CefInitialize(main_args, BuildCefSettings(), app.get(), 0)) {
        printf("Init Failed\n");
        return 1;
    }


    return 0;
}

static int qt_loop_source_id = -1;

void RunCEFLoop() {
    qt_loop_source_id = g_timeout_add(200, (GSourceFunc)handle_qt_event, 0);
    CefRunMessageLoop();
}

void ShutdownCEF() {
    CefShutdown();
}

void QuitCEFLoop() {
    qDebug() << __FUNCTION__;
    // Shut down CEF.
    if (qt_loop_source_id != -1) {
        g_source_remove(qt_loop_source_id);
        qt_loop_source_id = -1;
    }
    CefQuitMessageLoop();
}
