#include "LatexToolbarController.h"
#include "../views/MainWindow.h"
#include <QPlainTextEdit>

#include "LatexToolbarController.h"
#include "../views/MainWindow.h"
#include <QPlainTextEdit>

LatexToolbarController::LatexToolbarController(LatexToolbar *view, DocumentModel *model, MainWindow *mainWindow)
        : m_view(view), m_model(model), m_mainWindow(mainWindow) {
    connect(m_view, &LatexToolbar::commandInsertRequested, this, &LatexToolbarController::onCommandInsertRequested);
}

void LatexToolbarController::onCommandInsertRequested(const QString &command) {
    QPlainTextEdit *editor = m_mainWindow->getEditor();
    if (!editor) return;

    QTextCursor cursor = editor->textCursor();
    QString selectedText = cursor.selectedText();

    // Start of undo block
    cursor.beginEditBlock();

    if (selectedText.isEmpty()) {
        // No selection, just insert the command
        cursor.insertText(command);

        // Move cursor to appropriate position
        if (command.contains('\n')) {
            // For multi-line commands, move to after the first newline
            int newLinePos = command.indexOf('\n');
            if (newLinePos != -1) {
                cursor.setPosition(cursor.position() - (command.length() - newLinePos - 1));
            }
        } else {
            // For single-line commands, move inside brackets if present
            int bracketPos = command.indexOf("{}");
            if (bracketPos != -1) {
                cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, command.length() - bracketPos - 1);
            }
        }
    } else {
        // Text is selected, wrap it with the command
        int bracketPos = command.indexOf("{}");
        if (bracketPos != -1) {
            // Command has brackets, insert selected text between them
            QString before = command.left(bracketPos + 1);
            QString after = command.mid(bracketPos + 1);
            cursor.insertText(before + selectedText + after);
        } else {
            // Command doesn't have brackets, just prepend it
            cursor.insertText(command + selectedText);
        }
    }

    // End of undo block
    cursor.endEditBlock();

    editor->setTextCursor(cursor);
    editor->setFocus();

    // Update the model
    m_model->setContent(editor->toPlainText());
}
