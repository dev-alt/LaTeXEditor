#include "PreviewController.h"
#include <QDebug>

PreviewController::PreviewController(DocumentModel *model, PreviewWindow *view, QObject *parent)
        : QObject(parent), m_model(model), m_view(view) {
    connect(m_model, &DocumentModel::contentChanged, this, [this]() { updatePreview(); });
}

void PreviewController::updatePreview(const QString &content) {
    qDebug() << "PreviewController::updatePreview called";
    QString latexContent = content.isEmpty() ? m_model->getContent() : content;
    QString htmlContent = generatePreviewContent(latexContent);
    m_view->updatePreview(htmlContent);
    qDebug() << "Preview updated";
}

QString PreviewController::generatePreviewContent(const QString &latexContent) {
    // This is still a placeholder implementation.
    // In a real-world scenario, you would use a LaTeX to HTML converter here.
    QString htmlContent =
            "<html><body><h1>LaTeX Preview</h1><pre>" + latexContent.toHtmlEscaped() + "</pre></body></html>";
    return htmlContent;
}