#include "LaTeXHighlighter.h"

LaTeXHighlighter::LaTeXHighlighter(QTextDocument *parent)
        : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    // LaTeX commands
    commandFormat.setForeground(Qt::darkBlue);
    commandFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegularExpression(R"(\\[a-zA-Z]+)");
    rule.format = commandFormat;
    highlightingRules.append(rule);

    // LaTeX environments
    keywordFormat.setForeground(Qt::darkMagenta);
    keywordFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegularExpression(R"(\\begin\{.*\}|\\end\{.*\})");
    rule.format = keywordFormat;
    highlightingRules.append(rule);

    // Brackets
    bracketFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegularExpression(R"([\{\}\[\]])");
    rule.format = bracketFormat;
    highlightingRules.append(rule);

    // Comments
    commentFormat.setForeground(Qt::gray);
    rule.pattern = QRegularExpression(R"(%[^\n]*)");
    rule.format = commentFormat;
    highlightingRules.append(rule);
}

void LaTeXHighlighter::highlightBlock(const QString &text)
{
    for (const HighlightingRule &rule : qAsConst(highlightingRules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}
