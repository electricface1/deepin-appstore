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
#include <qcef_context.h>

#include "Shell.h"

int main(int argc, char *argv[]) {
    QNetworkProxyFactory::setUseSystemConfiguration(true);

    QCefGlobalSettings settings;
    settings.setRemoteDebug(true);
    QCefInit(argc, argv, settings);

    // The matching client version code in frontend(jssrc/services/AboutContentService.js)
    // should be sync with the ApplicationName here.
    Shell::setApplicationName("deepin-appstore");
    Shell::setApplicationDisplayName(QObject::tr("Deepin Store"));
    Shell::setApplicationVersion(SHELL_VERSION);
    Shell::setOrganizationDomain("deepin.org");
    Shell::setOrganizationName("deepin");

    Shell::loadDXcbPlugin();

    Shell shell(argc, argv);
    QCefBindApp(&shell);
    return shell.exec();
}
