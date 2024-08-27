#include "PreviewWindow.h"

#include <QVBoxLayout>

PreviewWindow::PreviewWindow(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);

#ifdef QT_WEBENGINEWIDGETS_LIB
    m_webView = new QWebEngineView(this);
    layout->addWidget(m_webView);
#else
    m_textBrowser = new QTextBrowser(this);
    layout->addWidget(m_textBrowser);
#endif

    setLayout(layout);
}

void PreviewWindow::updatePreview(const QString &htmlContent) {
#ifdef QT_WEBENGINEWIDGETS_LIB
    m_webView->setHtml(htmlContent);
#else
    m_textBrowser->setHtml(htmlContent);
#endif
}