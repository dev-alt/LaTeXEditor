#include "LaTeXHighlighter.h"
#include "ThemeManager.h"
#include <QRegularExpression>

LaTeXHighlighter::LaTeXHighlighter(QTextDocument *parent)
        : QSyntaxHighlighter(parent)
{
    setupHighlightingRules();
}

void LaTeXHighlighter::setupHighlightingRules()
{
    highlightingRules.clear();

    HighlightingRule rule;

    // LaTeX commands
    rule.pattern = QRegularExpression(R"(\\[a-zA-Z]+)");
    highlightingRules.append(rule);

    // LaTeX environments
    rule.pattern = QRegularExpression(R"(\\begin\{.*\}|\\end\{.*\})");
    highlightingRules.append(rule);

    // BibTeX entry types (@article, @book, etc.)
    rule.pattern = QRegularExpression(R"(@[a-zA-Z]+\{)");
    highlightingRules.append(rule);

    // BibTeX fields (author =, title =, etc.)
    rule.pattern = QRegularExpression(R"(\b[a-zA-Z]+\s*=)");
    highlightingRules.append(rule);

    // Brackets
    rule.pattern = QRegularExpression(R"([\{\}\[\]])");
    highlightingRules.append(rule);

    // Comments
    rule.pattern = QRegularExpression(R"(%[^\n]*)");
    highlightingRules.append(rule);

    // Apply initial theme
    updateTheme(ThemeManager::getInstance().getCurrentTheme());
}

void LaTeXHighlighter::updateTheme(const Theme &theme)
{
    if (highlightingRules.size() >= 6) {
        // LaTeX commands
        highlightingRules[0].format.setForeground(theme.commandColor);
        highlightingRules[0].format.setFontWeight(QFont::Bold);

        // LaTeX environments
        highlightingRules[1].format.setForeground(theme.environmentColor);
        highlightingRules[1].format.setFontWeight(QFont::Bold);

        // BibTeX entry types
        highlightingRules[2].format.setForeground(theme.commandColor);
        highlightingRules[2].format.setFontWeight(QFont::Bold);

        // BibTeX fields
        highlightingRules[3].format.setForeground(theme.environmentColor);

        // Brackets
        highlightingRules[4].format.setForeground(theme.bracketColor);

        // Comments
        highlightingRules[5].format.setForeground(theme.commentColor);
    }

    rehighlight();
}

void LaTeXHighlighter::highlightBlock(const QString &text)
{
    for (const HighlightingRule &rule : std::as_const(highlightingRules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}