#include "FindReplaceDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTextCursor>
#include <QMessageBox>

FindReplaceDialog::FindReplaceDialog(QPlainTextEdit *editor, QWidget *parent)
    : QDialog(parent), m_editor(editor) {

    setWindowTitle(tr("Find and Replace"));

    // Create widgets
    findLineEdit = new QLineEdit(this);
    replaceLineEdit = new QLineEdit(this);

    findNextButton = new QPushButton(tr("Find Next"), this);
    findPreviousButton = new QPushButton(tr("Find Previous"), this);
    replaceButton = new QPushButton(tr("Replace"), this);
    replaceAllButton = new QPushButton(tr("Replace All"), this);

    caseSensitiveCheckBox = new QCheckBox(tr("Case Sensitive"), this);
    wholeWordsCheckBox = new QCheckBox(tr("Whole Words"), this);

    // Layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Find section
    QHBoxLayout *findLayout = new QHBoxLayout();
    findLayout->addWidget(new QLabel(tr("Find:"), this));
    findLayout->addWidget(findLineEdit);
    mainLayout->addLayout(findLayout);

    // Replace section
    QHBoxLayout *replaceLayout = new QHBoxLayout();
    replaceLayout->addWidget(new QLabel(tr("Replace:"), this));
    replaceLayout->addWidget(replaceLineEdit);
    mainLayout->addLayout(replaceLayout);

    // Options
    QHBoxLayout *optionsLayout = new QHBoxLayout();
    optionsLayout->addWidget(caseSensitiveCheckBox);
    optionsLayout->addWidget(wholeWordsCheckBox);
    mainLayout->addLayout(optionsLayout);

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(findNextButton);
    buttonLayout->addWidget(findPreviousButton);
    buttonLayout->addWidget(replaceButton);
    buttonLayout->addWidget(replaceAllButton);
    mainLayout->addLayout(buttonLayout);

    // Connect signals
    connect(findNextButton, &QPushButton::clicked, this, &FindReplaceDialog::findNext);
    connect(findPreviousButton, &QPushButton::clicked, this, &FindReplaceDialog::findPrevious);
    connect(replaceButton, &QPushButton::clicked, this, &FindReplaceDialog::replace);
    connect(replaceAllButton, &QPushButton::clicked, this, &FindReplaceDialog::replaceAll);

    // Enable Enter key to trigger find
    connect(findLineEdit, &QLineEdit::returnPressed, this, &FindReplaceDialog::findNext);

    setLayout(mainLayout);
    resize(450, 150);
}

void FindReplaceDialog::findNext() {
    find(true);
}

void FindReplaceDialog::findPrevious() {
    find(false);
}

bool FindReplaceDialog::find(bool forward) {
    if (!m_editor) return false;

    QString searchString = findLineEdit->text();
    if (searchString.isEmpty()) return false;

    QTextDocument::FindFlags flags;
    if (!forward) {
        flags |= QTextDocument::FindBackward;
    }
    if (caseSensitiveCheckBox->isChecked()) {
        flags |= QTextDocument::FindCaseSensitively;
    }
    if (wholeWordsCheckBox->isChecked()) {
        flags |= QTextDocument::FindWholeWords;
    }

    bool found = m_editor->find(searchString, flags);

    if (!found) {
        // Wrap around
        QTextCursor cursor = m_editor->textCursor();
        if (forward) {
            cursor.movePosition(QTextCursor::Start);
        } else {
            cursor.movePosition(QTextCursor::End);
        }
        m_editor->setTextCursor(cursor);
        found = m_editor->find(searchString, flags);

        if (!found) {
            QMessageBox::information(this, tr("Find"), tr("Text not found."));
        }
    }

    return found;
}

void FindReplaceDialog::replace() {
    if (!m_editor) return;

    QTextCursor cursor = m_editor->textCursor();
    if (cursor.hasSelection()) {
        QString selectedText = cursor.selectedText();
        QString searchString = findLineEdit->text();

        // Check if the selected text matches the search string
        bool matches = false;
        if (caseSensitiveCheckBox->isChecked()) {
            matches = (selectedText == searchString);
        } else {
            matches = (selectedText.toLower() == searchString.toLower());
        }

        if (matches) {
            cursor.insertText(replaceLineEdit->text());
            findNext(); // Find next occurrence
        } else {
            findNext(); // Find first occurrence
        }
    } else {
        findNext(); // Find first occurrence
    }
}

void FindReplaceDialog::replaceAll() {
    if (!m_editor) return;

    QString searchString = findLineEdit->text();
    if (searchString.isEmpty()) return;

    int count = 0;

    // Move to start of document
    QTextCursor cursor = m_editor->textCursor();
    cursor.movePosition(QTextCursor::Start);
    m_editor->setTextCursor(cursor);

    m_editor->document()->blockSignals(true); // Prevent multiple updates

    while (find(true)) {
        QTextCursor cursor = m_editor->textCursor();
        if (cursor.hasSelection()) {
            cursor.insertText(replaceLineEdit->text());
            count++;
        }
    }

    m_editor->document()->blockSignals(false);
    m_editor->document()->contentsChanged(); // Trigger single update

    QMessageBox::information(this, tr("Replace All"),
                           tr("Replaced %1 occurrence(s).").arg(count));
}
