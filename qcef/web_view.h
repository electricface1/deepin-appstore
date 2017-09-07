// Copyright (c) 2017 LiuLang. All rights reserved.
// Use of this source is governed by General Public License that can be found
// in the LICENSE file.

#ifndef SHELL_WEB_VIEW_H
#define SHELL_WEB_VIEW_H

#include <qcef_web_view.h>

class WebView : public QCefWebView {
  Q_OBJECT

 public:
  explicit WebView(QWidget* parent = nullptr);

 signals:
  void titleChanged(const QString& title);
};


#endif //SHELL_WEB_VIEW_H
