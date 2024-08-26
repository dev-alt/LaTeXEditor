#include "EditorController.h"

EditorController::EditorController(DocumentModel *model, MainWindow *view, QObject *parent)
        : QObject(parent), m_model(model), m_view(view) {
    connect(m_view->getEditor(), &QPlainTextEdit::textChanged, this, &EditorController::onEditorTextChanged);
    connect(m_model, &DocumentModel::contentChanged, this, &EditorController::onModelContentChanged);
}

void EditorController::onEditorTextChanged() {
    m_model->setContent(m_view->getEditor()->toPlainText());
}

void EditorController::onModelContentChanged() {
    m_view->getEditor()->setPlainText(m_model->getContent());
}