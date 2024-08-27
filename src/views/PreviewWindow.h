#ifndef PREVIEWWINDOW_H
#define PREVIEWWINDOW_H

#include <QWidget>
#include <QTextBrowser>

#ifdef QT_WEBENGINEWIDGETS_LIB
#include <QWebEngineView>
#endif

class PreviewWindow : public QWidget {
Q_OBJECT

public:
    explicit PreviewWindow(QWidget *parent = nullptr);

    void updatePreview(const QString &htmlContent);

private:
#ifdef QT_WEBENGINEWIDGETS_LIB
    QWebEngineView *m_webView;
#else
    QTextBrowser *m_textBrowser;
#endif
};

#endif // PREVIEWWINDOW_H