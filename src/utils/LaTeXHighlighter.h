#ifndef LATEXHIGHLIGHTER_H
#define LATEXHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include "models/Theme.h"

class LaTeXHighlighter : public QSyntaxHighlighter
{
Q_OBJECT

public:
    LaTeXHighlighter(QTextDocument *parent = nullptr);
    void updateTheme(const Theme &theme);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    void setupHighlightingRules();
};

#endif // LATEXHIGHLIGHTER_H