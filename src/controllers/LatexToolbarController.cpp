#include "LatexToolbarController.h"
#include "../views/MainWindow.h"
#include <QPlainTextEdit>

LatexToolbarController::LatexToolbarController(LatexToolbar *view, DocumentModel *model, MainWindow *mainWindow)
        : m_view(view), m_model(model), m_mainWindow(mainWindow) {
    connect(m_view, &LatexToolbar::commandInsertRequested, this, &LatexToolbarController::onCommandInsertRequested);
}

void LatexToolbarController::onCommandInsertRequested(const QString &command) {
    QPlainTextEdit *editor = m_mainWindow->getEditor();
    if (editor) {
        QTextCursor cursor = editor->textCursor();
        cursor.insertText(command);

        // Move cursor inside brackets if present
        int bracketPos = command.indexOf("{}");
        if (bracketPos != -1) {
            cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, command.length() - bracketPos - 1);
            editor->setTextCursor(cursor);
        }

        // Update the model
        m_model->setContent(editor->toPlainText());
    }
}