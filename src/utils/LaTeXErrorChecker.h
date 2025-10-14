#ifndef LATEXERRORCHECKER_H
#define LATEXERRORCHECKER_H

#include <QString>
#include <QVector>
#include <QObject>

struct LaTeXError {
    enum ErrorType {
        UnmatchedBrace,
        UnmatchedBracket,
        UnmatchedEnvironment,
        UnmatchedMathDelimiter,
        InvalidCommand,
        MissingArgument,
        UnknownCommand,
        MissingPackage,
        UsePackageAfterBeginDocument,
        InvalidArgumentCount,
        DeprecatedCommand,
        MathModeRequired,
        TextModeRequired
    };

    ErrorType type;
    int line;
    int column;
    QString message;
    QString context; // The problematic text

    LaTeXError(ErrorType t, int l, int c, const QString &msg, const QString &ctx = "")
        : type(t), line(l), column(c), message(msg), context(ctx) {}
};

class LaTeXErrorChecker : public QObject {
    Q_OBJECT

public:
    explicit LaTeXErrorChecker(QObject *parent = nullptr);

    QVector<LaTeXError> checkDocument(const QString &content);

private:
    struct BraceInfo {
        int line;
        int column;
        char type; // '{', '[', etc.
    };

    struct EnvironmentInfo {
        QString name;
        int line;
        int column;
    };

    QVector<LaTeXError> checkBraces(const QString &content);
    QVector<LaTeXError> checkEnvironments(const QString &content);
    QVector<LaTeXError> checkMathDelimiters(const QString &content);
    QVector<LaTeXError> checkCommands(const QString &content);
    QVector<LaTeXError> checkPackages(const QString &content);
    QVector<LaTeXError> checkCommonMistakes(const QString &content);

    bool isInComment(const QString &line, int position);
    void getLineColumn(const QString &content, int position, int &line, int &column);

    // Command and package databases
    struct CommandInfo {
        QString name;
        int requiredArgs;
        int optionalArgs;
        QString requiredPackage;
        bool mathModeOnly;
        bool textModeOnly;
        QString deprecatedReplacement;
    };

    void initializeCommandDatabase();
    void initializePackageDatabase();
    QMap<QString, CommandInfo> m_commandDatabase;
    QMap<QString, QString> m_packageCommands; // command -> package
    QSet<QString> m_mathCommands;
    QSet<QString> m_deprecatedCommands;
};

#endif // LATEXERRORCHECKER_H
