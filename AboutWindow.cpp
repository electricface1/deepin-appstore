/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/
#include <QDebug>
#include <QDesktopServices>
#include <DPlatformWindowHandle>
#include <QLayout>
#include <QPushButton>
#include <QKeyEvent>
#include <qcef_web_page.h>
#include <qcef_web_settings.h>

#include "AboutWindow.h"
#include "WebWidget.h"

AboutWindow::AboutWindow(QWidget *parent) : QFrame(parent),
                                            contentWidth(380), contentHeight(390) {
    this->setWindowModality(Qt::WindowModality::ApplicationModal);
    this->setAutoFillBackground(true);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setFixedSize(this->contentWidth, this->contentHeight);
//    this->setStyleSheet("AboutWindow { background: transparent }");

    this->content = new WebView();
    this->content->page()->settings()->setWebSecurity(QCefWebSettings::StateDisabled);
    this->content->setFixedSize(this->contentWidth, this->contentHeight);

    const auto closeBtn = new QPushButton(this);
    closeBtn->setCheckable(true);
    closeBtn->setFixedSize(25, 24);
    closeBtn->setStyleSheet(
        "QPushButton { border: 0; outline: none; background: url(':/res/close_small_normal.png'); }"
        "QPushButton:hover { background: url(':/res/close_small_hover.png'); }"
        "QPushButton:pressed { background: url(':/res/close_small_press.png'); }"
    );
    closeBtn->setFlat(true);

    // smaller shadow
    auto layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->setObjectName("horizontalLayout");
    this->setLayout(layout);
    layout->addWidget(closeBtn, 0, Qt::AlignRight);
    layout->addWidget(this->content);

    connect(closeBtn, &QPushButton::clicked, [this]() {
        this->close();
    });

    DPlatformWindowHandle handler(this);
    handler.setWindowRadius(5);
}

void AboutWindow::setContent(const QString& html) {
    qDebug() << "About window:" << html;
    QString m = html;
    m = m.replace(":/", "qrc:/");
    this->content->page()->setHtml(m, QUrl("https://deepin.org"));
}

AboutWindow::~AboutWindow() {

}

void AboutWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape &&
        event->modifiers() == Qt::NoModifier) {
        this->close();
    }
}
