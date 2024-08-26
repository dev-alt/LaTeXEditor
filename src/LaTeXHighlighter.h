#ifndef LATEXHIGHLIGHTER_H
#define LATEXHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>

class LaTeXHighlighter : public QSyntaxHighlighter
{
Q_OBJECT

public:
    LaTeXHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QTextCharFormat keywordFormat;
    QTextCharFormat commandFormat;
    QTextCharFormat bracketFormat;
    QTextCharFormat commentFormat;
};

#endif // LATEXHIGHLIGHTER_H