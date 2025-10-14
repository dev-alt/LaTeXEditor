// SpellChecker.h
#ifndef SPELLCHECKER_H
#define SPELLCHECKER_H

#include <QString>
#include <QStringList>
#include <QSet>
#include <QObject>

#ifdef HAVE_HUNSPELL
#include <hunspell/hunspell.hxx>
#endif

class SpellChecker : public QObject {
Q_OBJECT

public:
    explicit SpellChecker(QObject *parent = nullptr);
    ~SpellChecker();

    // Initialize with dictionary files
    bool initialize(const QString &affixPath, const QString &dictionaryPath);
    bool isInitialized() const;

    // Check if a word is spelled correctly
    bool isCorrect(const QString &word) const;

    // Get suggestions for a misspelled word
    QStringList suggestions(const QString &word) const;

    // Add word to personal dictionary
    void addToPersonalDictionary(const QString &word);
    bool isInPersonalDictionary(const QString &word) const;

    // Ignore word for this session
    void ignoreWord(const QString &word);
    bool isIgnored(const QString &word) const;

    // Clear session-specific data
    void clearIgnoredWords();

    // Load/save personal dictionary
    bool loadPersonalDictionary(const QString &path);
    bool savePersonalDictionary(const QString &path) const;

    // Get default dictionary paths
    static QString getDefaultAffixPath();
    static QString getDefaultDictionaryPath();
    static QString getDefaultPersonalDictionaryPath();

signals:
    void dictionaryChanged();

private:
#ifdef HAVE_HUNSPELL
    Hunspell *m_hunspell;
#else
    void *m_hunspell; // Placeholder when Hunspell is not available
#endif

    bool m_initialized;
    QSet<QString> m_personalDictionary;
    QSet<QString> m_ignoredWords;
    QString m_encoding;

    // Helper to convert between QString and Hunspell encoding
    QByteArray toHunspellEncoding(const QString &word) const;
    QString fromHunspellEncoding(const char *word) const;
};

#endif // SPELLCHECKER_H
