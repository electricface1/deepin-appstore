/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/
#include <QDebug>
#include <QNetworkProxyFactory>

#include "MainWindow.h"
#include "Shell.h"
#include "cef_init.h"

int main(int argc, char *argv[]) {
    QNetworkProxyFactory::setUseSystemConfiguration(true);

    Shell::setApplicationName("DeepinStore");
    Shell::setApplicationDisplayName("Deepin Store");
    Shell::setApplicationVersion(SHELL_VERSION);
    Shell::setOrganizationDomain("deepin.org");
    Shell::setOrganizationName("Deepin");

    InitCEF(argc, argv);

    Shell shell(argc, argv);

    MainWindow win;
    win.setUrl(shell.initUrl);
    win.show();
    win.polish();

    RunCEFLoop();

    ShutdownCEF();
    return 0;
}
