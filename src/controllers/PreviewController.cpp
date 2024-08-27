#include "PreviewController.h"
#include "../models/DocumentModel.h"
#include "../views/PreviewWindow.h"

PreviewController::PreviewController(DocumentModel *model, PreviewWindow *view, QObject *parent)
        : QObject(parent), m_model(model), m_view(view) {

    connect(m_model, &DocumentModel::contentChanged, this, &PreviewController::updatePreview);
}

void PreviewController::updatePreview() {
    qDebug() << "PreviewController::updatePreview called";
    QString newContent = m_model->getContent();
    if (newContent != m_lastContent) {
        m_lastContent = newContent;
        QString htmlContent = generatePreviewContent(newContent);
        m_view->updatePreview(htmlContent);
        qDebug() << "Preview updated";
    } else {
        qDebug() << "Preview update skipped (content unchanged)";
    }
    qDebug() << "PreviewController::updatePreview finished";
}


QString PreviewController::generatePreviewContent(const QString &latexContent) {
    // This is a placeholder implementation. In a real-world scenario,
    // you would use a LaTeX to HTML converter here.
    return "<html><body><pre>" + latexContent.toHtmlEscaped() + "</pre></body></html>";
}