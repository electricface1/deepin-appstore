// Copyright (c) 2017 LiuLang. All rights reserved.
// Use of this source is governed by General Public License that can be found
// in the LICENSE file.

#include "web_view.h"

#include <QWebChannel>
#include <qcef_web_page.h>
#include "Bridge.h"

WebView::WebView(QWidget* parent) : QCefWebView(parent) {
  auto channel = this->page()->webChannel();
  const auto bridge = new Bridge(this);
  channel->registerObject("bridge", bridge);

  connect(this->page(), &QCefWebPage::titleChanged,
          this, &WebView::titleChanged);
}