#include "PreviewController.h"
#include <QDebug>
#include <QRegularExpression>

PreviewController::PreviewController(DocumentModel *model, PreviewWindow *view, QObject *parent)
        : QObject(parent), m_model(model), m_view(view) {

    // Initialize converter
    m_converter = new LaTeXToHtmlConverter();

    // Create timer for debounced updates
    m_updateTimer = new QTimer(this);
    m_updateTimer->setSingleShot(true);
    m_updateTimer->setInterval(500); // 500ms delay
    connect(m_updateTimer, &QTimer::timeout, this, [this]() { updatePreview(); });

    // Connect model changes to schedule update
    connect(m_model, &DocumentModel::contentChanged, this, &PreviewController::schedulePreviewUpdate);

    // Initial preview
    updatePreview();
}

void PreviewController::updatePreview(const QString &content) {
    qDebug() << "PreviewController::updatePreview called";
    QString latexContent = content.isEmpty() ? m_model->getContent() : content;
    QString htmlContent = generatePreviewContent(latexContent);
    m_view->updatePreview(htmlContent);
    qDebug() << "Preview updated";
}

void PreviewController::schedulePreviewUpdate() {
    // Restart the timer - this debounces rapid changes
    m_updateTimer->start();
}

QString PreviewController::generatePreviewContent(const QString &latexContent) {
    // Use the enhanced converter (with CDN for MathJax)
    return m_converter->convertToHtml(latexContent, true);
}