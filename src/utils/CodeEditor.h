#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>
#include <QObject>
#include <QVector>
#include <QMenu>
#include "../utils/LaTeXErrorChecker.h"

class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;
class SpellChecker;

class LineNumberArea;

class CodeEditor : public QPlainTextEdit {
Q_OBJECT

public:
    CodeEditor(QWidget *parent = nullptr);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

    void setErrors(const QVector<LaTeXError> &errors);
    void clearErrors();
    QVector<LaTeXError> getErrors() const { return m_errors; }

    void setSpellChecker(SpellChecker *spellChecker);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);
    void highlightErrors();

private:
    QWidget *lineNumberArea;
    QVector<LaTeXError> m_errors;
    SpellChecker *m_spellChecker;

    QString getWordUnderCursor() const;
};

class LineNumberArea : public QWidget {
public:
    LineNumberArea(CodeEditor *editor) : QWidget(editor), codeEditor(editor) {}

    QSize sizeHint() const override {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    CodeEditor *codeEditor;
};

#endif // CODEEDITOR_H
