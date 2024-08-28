#ifndef PREVIEWWINDOW_H
#define PREVIEWWINDOW_H

#include <QWidget>
#include <QTextBrowser>
#include "../models/Theme.h"

#ifdef QT_WEBENGINEWIDGETS_LIB
#include <QWebEngineView>
#endif

class PreviewWindow : public QWidget {
Q_OBJECT

public:
    explicit PreviewWindow(QWidget *parent = nullptr);

public slots:
    void updatePreview(const QString &htmlContent);

    void updateTheme(const Theme &theme);

private:
#ifdef QT_WEBENGINEWIDGETS_LIB
    QWebEngineView *m_webView;
#else
    QTextBrowser *m_textBrowser;
#endif
};

#endif // PREVIEWWINDOW_H