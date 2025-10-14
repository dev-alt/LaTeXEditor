// SpellCheckHighlighter.h
#ifndef SPELLCHECKHIGHLIGHTER_H
#define SPELLCHECKHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include "SpellChecker.h"

class SpellCheckHighlighter : public QSyntaxHighlighter {
Q_OBJECT

public:
    explicit SpellCheckHighlighter(SpellChecker *spellChecker, QTextDocument *parent = nullptr);

    void setEnabled(bool enabled);
    bool isEnabled() const;

    void rehighlight();

protected:
    void highlightBlock(const QString &text) override;

private:
    SpellChecker *m_spellChecker;
    bool m_enabled;
    QTextCharFormat m_misspelledFormat;

    // Extract words from text, skipping LaTeX commands
    struct WordPosition {
        QString word;
        int start;
        int length;
    };
    QList<WordPosition> extractWords(const QString &text) const;

    // Check if position is inside LaTeX command or environment
    bool isInsideLatexCommand(const QString &text, int position) const;
    bool isInsideLatexEnvironment(const QString &text, int position) const;
};

#endif // SPELLCHECKHIGHLIGHTER_H
