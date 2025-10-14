// SpellChecker.cpp
#include "SpellChecker.h"
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QCoreApplication>

SpellChecker::SpellChecker(QObject *parent)
    : QObject(parent)
    , m_hunspell(nullptr)
    , m_initialized(false)
    , m_encoding("UTF-8")
{
}

SpellChecker::~SpellChecker() {
#ifdef HAVE_HUNSPELL
    if (m_hunspell) {
        delete m_hunspell;
        m_hunspell = nullptr;
    }
#endif
}

bool SpellChecker::initialize(const QString &affixPath, const QString &dictionaryPath) {
#ifdef HAVE_HUNSPELL
    if (m_hunspell) {
        delete m_hunspell;
        m_hunspell = nullptr;
    }

    QByteArray affixBytes = affixPath.toUtf8();
    QByteArray dictBytes = dictionaryPath.toUtf8();

    try {
        m_hunspell = new Hunspell(affixBytes.constData(), dictBytes.constData());
        m_encoding = QString::fromLatin1(m_hunspell->get_dic_encoding());
        m_initialized = true;
        qDebug() << "Hunspell initialized with encoding:" << m_encoding;
        return true;
    } catch (...) {
        qWarning() << "Failed to initialize Hunspell with" << affixPath << "and" << dictionaryPath;
        m_initialized = false;
        return false;
    }
#else
    qWarning() << "Hunspell support not compiled. Spell checking disabled.";
    qWarning() << "Requested paths:" << affixPath << dictionaryPath;
    m_initialized = false;
    return false;
#endif
}

bool SpellChecker::isInitialized() const {
    return m_initialized;
}

bool SpellChecker::isCorrect(const QString &word) const {
    if (!m_initialized || word.isEmpty()) {
        return true; // Don't mark as incorrect if not initialized
    }

    // Check personal dictionary first
    if (m_personalDictionary.contains(word)) {
        return true;
    }

    // Check ignored words
    if (m_ignoredWords.contains(word)) {
        return true;
    }

#ifdef HAVE_HUNSPELL
    if (m_hunspell) {
        QByteArray encodedWord = toHunspellEncoding(word);
        return m_hunspell->spell(encodedWord.constData()) != 0;
    }
#endif

    return true;
}

QStringList SpellChecker::suggestions(const QString &word) const {
    QStringList suggestionList;

    if (!m_initialized || word.isEmpty()) {
        return suggestionList;
    }

#ifdef HAVE_HUNSPELL
    if (m_hunspell) {
        QByteArray encodedWord = toHunspellEncoding(word);
        std::vector<std::string> suggestions = m_hunspell->suggest(encodedWord.constData());

        for (const std::string &suggestion : suggestions) {
            suggestionList.append(fromHunspellEncoding(suggestion.c_str()));
        }
    }
#endif

    return suggestionList;
}

void SpellChecker::addToPersonalDictionary(const QString &word) {
    if (!word.isEmpty()) {
        m_personalDictionary.insert(word);
        emit dictionaryChanged();
    }
}

bool SpellChecker::isInPersonalDictionary(const QString &word) const {
    return m_personalDictionary.contains(word);
}

void SpellChecker::ignoreWord(const QString &word) {
    if (!word.isEmpty()) {
        m_ignoredWords.insert(word);
        emit dictionaryChanged();
    }
}

bool SpellChecker::isIgnored(const QString &word) const {
    return m_ignoredWords.contains(word);
}

void SpellChecker::clearIgnoredWords() {
    m_ignoredWords.clear();
    emit dictionaryChanged();
}

bool SpellChecker::loadPersonalDictionary(const QString &path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "No personal dictionary found at:" << path;
        return false;
    }

    QTextStream in(&file);
    m_personalDictionary.clear();

    while (!in.atEnd()) {
        QString word = in.readLine().trimmed();
        if (!word.isEmpty()) {
            m_personalDictionary.insert(word);
        }
    }

    file.close();
    qDebug() << "Loaded" << m_personalDictionary.size() << "words from personal dictionary";
    emit dictionaryChanged();
    return true;
}

bool SpellChecker::savePersonalDictionary(const QString &path) const {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to save personal dictionary to:" << path;
        return false;
    }

    QTextStream out(&file);
    QStringList sortedWords = m_personalDictionary.values();
    sortedWords.sort();

    for (const QString &word : sortedWords) {
        out << word << "\n";
    }

    file.close();
    qDebug() << "Saved" << m_personalDictionary.size() << "words to personal dictionary";
    return true;
}

QString SpellChecker::getDefaultAffixPath() {
    // Try common locations for Hunspell dictionaries
    QStringList searchPaths = {
        "/usr/share/hunspell/en_US.aff",
        "/usr/share/myspell/en_US.aff",
        "/usr/local/share/hunspell/en_US.aff",
        QCoreApplication::applicationDirPath() + "/dictionaries/en_US.aff",
        "./dictionaries/en_US.aff"
    };

    for (const QString &path : searchPaths) {
        if (QFile::exists(path)) {
            return path;
        }
    }

    return QString();
}

QString SpellChecker::getDefaultDictionaryPath() {
    // Try common locations for Hunspell dictionaries
    QStringList searchPaths = {
        "/usr/share/hunspell/en_US.dic",
        "/usr/share/myspell/en_US.dic",
        "/usr/local/share/hunspell/en_US.dic",
        QCoreApplication::applicationDirPath() + "/dictionaries/en_US.dic",
        "./dictionaries/en_US.dic"
    };

    for (const QString &path : searchPaths) {
        if (QFile::exists(path)) {
            return path;
        }
    }

    return QString();
}

QString SpellChecker::getDefaultPersonalDictionaryPath() {
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataPath);
    return QDir(dataPath).filePath("personal_dictionary.txt");
}

QByteArray SpellChecker::toHunspellEncoding(const QString &word) const {
    if (m_encoding == "UTF-8") {
        return word.toUtf8();
    } else {
        return word.toLatin1();
    }
}

QString SpellChecker::fromHunspellEncoding(const char *word) const {
    if (m_encoding == "UTF-8") {
        return QString::fromUtf8(word);
    } else {
        return QString::fromLatin1(word);
    }
}
