#include "CodeEditor.h"
#include "SpellChecker.h"
#include <QPainter>
#include <QTextBlock>
#include <QContextMenuEvent>
#include <QTextCursor>

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent), m_spellChecker(nullptr) {
    lineNumberArea = new LineNumberArea(this);

    connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &CodeEditor::updateRequest, this, &CodeEditor::updateLineNumberArea);
    connect(this, &CodeEditor::cursorPositionChanged, this, &CodeEditor::highlightCurrentLine);
    connect(this, &CodeEditor::cursorPositionChanged, this, &CodeEditor::highlightErrors);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}

int CodeEditor::lineNumberAreaWidth() {
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    // Add space for error indicator (!) plus line number
    int space = 20 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
    return space;
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */) {
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy) {
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void CodeEditor::resizeEvent(QResizeEvent *e) {
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::highlightCurrentLine() {
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::yellow).lighter(160);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    // Add error highlights
    for (const LaTeXError &error : m_errors) {
        QTextBlock block = document()->findBlockByLineNumber(error.line);
        if (block.isValid()) {
            QTextEdit::ExtraSelection selection;
            selection.format.setUnderlineColor(Qt::red);
            selection.format.setUnderlineStyle(QTextCharFormat::WaveUnderline);

            selection.cursor = QTextCursor(block);
            selection.cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, error.column);

            // Underline the problematic area (or whole line if column is 0)
            if (error.column == 0) {
                selection.cursor.select(QTextCursor::LineUnderCursor);
            } else {
                // Underline a few characters or until end of word
                int endPos = error.column + qMax(1, error.context.length());
                selection.cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor,
                                             qMin(endPos - error.column, block.length() - error.column));
            }

            extraSelections.append(selection);
        }
    }

    setExtraSelections(extraSelections);
}

void CodeEditor::highlightErrors() {
    highlightCurrentLine(); // Re-apply both highlights
}

void CodeEditor::setErrors(const QVector<LaTeXError> &errors) {
    m_errors = errors;
    highlightErrors();
    lineNumberArea->update(); // Update to show error icons
}

void CodeEditor::clearErrors() {
    m_errors.clear();
    highlightErrors();
    lineNumberArea->update();
}

void CodeEditor::setSpellChecker(SpellChecker *spellChecker) {
    m_spellChecker = spellChecker;
}

void CodeEditor::contextMenuEvent(QContextMenuEvent *event) {
    QMenu *menu = createStandardContextMenu();

    // Add spell checking suggestions if spell checker is available
    if (m_spellChecker && m_spellChecker->isInitialized()) {
        QString word = getWordUnderCursor();

        if (!word.isEmpty() && !m_spellChecker->isCorrect(word)) {
            // Get suggestions
            QStringList suggestions = m_spellChecker->suggestions(word);

            // Add separator before spelling suggestions
            menu->insertSeparator(menu->actions().first());

            // Add "Add to Dictionary" action
            QAction *addToDictAction = new QAction(tr("Add '%1' to Dictionary").arg(word), menu);
            connect(addToDictAction, &QAction::triggered, [this, word]() {
                if (m_spellChecker) {
                    m_spellChecker->addToPersonalDictionary(word);
                }
            });
            menu->insertAction(menu->actions().first(), addToDictAction);

            // Add "Ignore" action
            QAction *ignoreAction = new QAction(tr("Ignore '%1'").arg(word), menu);
            connect(ignoreAction, &QAction::triggered, [this, word]() {
                if (m_spellChecker) {
                    m_spellChecker->ignoreWord(word);
                }
            });
            menu->insertAction(menu->actions().first(), ignoreAction);

            menu->insertSeparator(menu->actions().first());

            // Add suggestions (limit to 5)
            int count = 0;
            for (const QString &suggestion : suggestions) {
                if (count >= 5) break;

                QAction *suggestionAction = new QAction(suggestion, menu);
                connect(suggestionAction, &QAction::triggered, [this, word, suggestion]() {
                    // Replace the misspelled word with the suggestion
                    QTextCursor cursor = textCursor();
                    cursor.select(QTextCursor::WordUnderCursor);
                    cursor.insertText(suggestion);
                });
                menu->insertAction(menu->actions().first(), suggestionAction);
                count++;
            }

            if (suggestions.isEmpty()) {
                QAction *noSuggestionsAction = new QAction(tr("(No suggestions)"), menu);
                noSuggestionsAction->setEnabled(false);
                menu->insertAction(menu->actions().first(), noSuggestionsAction);
            }
        }
    }

    menu->exec(event->globalPos());
    delete menu;
}

QString CodeEditor::getWordUnderCursor() const {
    QTextCursor cursor = textCursor();
    cursor.select(QTextCursor::WordUnderCursor);
    return cursor.selectedText();
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event) {
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);

            // Check if this line has errors
            bool hasError = false;
            for (const LaTeXError &error : m_errors) {
                if (error.line == blockNumber) {
                    hasError = true;
                    break;
                }
            }

            if (hasError) {
                painter.setPen(Qt::red);
                painter.setFont(QFont(painter.font().family(), painter.font().pointSize(), QFont::Bold));
                // Draw error indicator
                painter.drawText(0, top, 16, fontMetrics().height(), Qt::AlignLeft, "!");
                painter.setFont(QFont(painter.font().family(), painter.font().pointSize(), QFont::Normal));
            } else {
                painter.setPen(Qt::black);
            }

            painter.drawText(16, top, lineNumberArea->width() - 16, fontMetrics().height(),
                           Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}
