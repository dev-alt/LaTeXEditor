// SpellCheckHighlighter.cpp
#include "SpellCheckHighlighter.h"
#include <QTextDocument>
#include <QDebug>

SpellCheckHighlighter::SpellCheckHighlighter(SpellChecker *spellChecker, QTextDocument *parent)
    : QSyntaxHighlighter(parent)
    , m_spellChecker(spellChecker)
    , m_enabled(false)
{
    // Format for misspelled words: red wavy underline
    m_misspelledFormat.setUnderlineColor(Qt::red);
    m_misspelledFormat.setUnderlineStyle(QTextCharFormat::WaveUnderline);

    // Connect to spell checker changes
    if (m_spellChecker) {
        connect(m_spellChecker, &SpellChecker::dictionaryChanged,
                this, &SpellCheckHighlighter::rehighlight);
    }
}

void SpellCheckHighlighter::setEnabled(bool enabled) {
    if (m_enabled != enabled) {
        m_enabled = enabled;
        rehighlight();
    }
}

bool SpellCheckHighlighter::isEnabled() const {
    return m_enabled;
}

void SpellCheckHighlighter::highlightBlock(const QString &text) {
    if (!m_enabled || !m_spellChecker || !m_spellChecker->isInitialized()) {
        return;
    }

    // Extract words from the text
    QList<WordPosition> words = extractWords(text);

    // Check each word and highlight if misspelled
    for (const WordPosition &wordPos : words) {
        // Skip if inside LaTeX command or environment
        if (isInsideLatexCommand(text, wordPos.start)) {
            continue;
        }

        // Skip words that are all uppercase (likely acronyms)
        if (wordPos.word == wordPos.word.toUpper() && wordPos.word.length() > 1) {
            continue;
        }

        // Skip words with numbers
        if (wordPos.word.contains(QRegularExpression("[0-9]"))) {
            continue;
        }

        // Check spelling
        if (!m_spellChecker->isCorrect(wordPos.word)) {
            setFormat(wordPos.start, wordPos.length, m_misspelledFormat);
        }
    }
}

QList<SpellCheckHighlighter::WordPosition> SpellCheckHighlighter::extractWords(const QString &text) const {
    QList<WordPosition> words;

    // Match words (sequences of letters, allowing apostrophes and hyphens within words)
    QRegularExpression wordRegex("\\b[a-zA-Z]+(?:['-][a-zA-Z]+)*\\b");
    QRegularExpressionMatchIterator it = wordRegex.globalMatch(text);

    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        WordPosition wp;
        wp.word = match.captured(0);
        wp.start = match.capturedStart(0);
        wp.length = match.capturedLength(0);
        words.append(wp);
    }

    return words;
}

bool SpellCheckHighlighter::isInsideLatexCommand(const QString &text, int position) const {
    // Check if we're inside a LaTeX command (e.g., \command{text})

    // Find the last backslash before this position
    int lastBackslash = text.lastIndexOf('\\', position - 1);
    if (lastBackslash == -1) {
        return false;
    }

    // Check if there's a closing brace after the position
    int openBrace = text.indexOf('{', lastBackslash);
    if (openBrace == -1 || openBrace >= position) {
        return false;
    }

    int closeBrace = text.indexOf('}', position);
    if (closeBrace == -1) {
        return true; // Inside an unclosed command
    }

    // We're inside a command if we're between { and }
    return position > openBrace && position < closeBrace;
}

bool SpellCheckHighlighter::isInsideLatexEnvironment(const QString &text, int position) const {
    // Check if we're inside certain LaTeX environments that shouldn't be spell-checked
    // Examples: \verb, \url, \cite, \ref, \label

    QStringList skipCommands = {"\\verb", "\\url", "\\cite", "\\ref", "\\label",
                                "\\includegraphics", "\\input", "\\include"};

    for (const QString &cmd : skipCommands) {
        int cmdPos = text.lastIndexOf(cmd, position - 1);
        if (cmdPos != -1) {
            // Check if we're still within this command
            int openBrace = text.indexOf('{', cmdPos);
            int closeBrace = text.indexOf('}', cmdPos);

            if (openBrace != -1 && closeBrace != -1 &&
                position > openBrace && position < closeBrace) {
                return true;
            }
        }
    }

    return false;
}
