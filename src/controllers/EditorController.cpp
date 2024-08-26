#include "EditorController.h"

EditorController::EditorController(DocumentModel *model, MainWindow *view, QObject *parent)
        : QObject(parent), m_model(model), m_view(view), m_themeManager(ThemeManager::getInstance())
{
    connect(m_view->getEditor(), &QPlainTextEdit::textChanged, this, &EditorController::onEditorTextChanged);
    connect(m_model, &DocumentModel::contentChanged, this, &EditorController::onModelContentChanged);
    connect(m_view, &MainWindow::themeChangeRequested, this, &EditorController::onThemeChangeRequested);

    // Initialize with default theme
    m_themeManager.applyTheme("Light");
}

void EditorController::onEditorTextChanged()
{
    m_model->setContent(m_view->getEditor()->toPlainText());
}

void EditorController::onModelContentChanged()
{
    m_view->getEditor()->setPlainText(m_model->getContent());
}

void EditorController::onThemeChangeRequested(const QString &themeName)
{
    m_themeManager.applyTheme(themeName);
    m_view->updateTheme(m_themeManager.getCurrentTheme());
}