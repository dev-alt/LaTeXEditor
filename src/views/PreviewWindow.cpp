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

void PreviewWindow::updateTheme(const Theme &theme) {
    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, theme.windowColor);
    palette.setColor(QPalette::WindowText, theme.textColor);
    palette.setColor(QPalette::Base, theme.baseColor);
    palette.setColor(QPalette::Text, theme.textColor);

    this->setPalette(palette);

#ifdef QT_WEBENGINEWIDGETS_LIB
    // For QWebEngineView, we might need to inject CSS to change the theme
    QString css = QString("body { background-color: %1; color: %2; }")
                      .arg(theme.baseColor.name())
                      .arg(theme.textColor.name());
    m_webView->page()->runJavaScript(QString("document.body.style.cssText = '%1';").arg(css));
#else
    m_textBrowser->setPalette(palette);
#endif
}