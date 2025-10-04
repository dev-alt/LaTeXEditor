#include "LaTeXErrorChecker.h"
#include <QRegularExpression>
#include <QStack>

LaTeXErrorChecker::LaTeXErrorChecker(QObject *parent) : QObject(parent) {}

QVector<LaTeXError> LaTeXErrorChecker::checkDocument(const QString &content) {
    QVector<LaTeXError> errors;

    // Check braces and brackets
    errors.append(checkBraces(content));

    // Check environments
    errors.append(checkEnvironments(content));

    // Check math delimiters
    errors.append(checkMathDelimiters(content));

    return errors;
}

bool LaTeXErrorChecker::isInComment(const QString &line, int position) {
    // Check if position is after a % that's not escaped
    for (int i = 0; i < position && i < line.length(); ++i) {
        if (line[i] == '%' && (i == 0 || line[i-1] != '\\')) {
            return true;
        }
    }
    return false;
}

void LaTeXErrorChecker::getLineColumn(const QString &content, int position, int &line, int &column) {
    line = 0;
    column = 0;

    for (int i = 0; i < position && i < content.length(); ++i) {
        if (content[i] == '\n') {
            line++;
            column = 0;
        } else {
            column++;
        }
    }
}

QVector<LaTeXError> LaTeXErrorChecker::checkBraces(const QString &content) {
    QVector<LaTeXError> errors;
    QStack<BraceInfo> braceStack;
    QStack<BraceInfo> bracketStack;

    QStringList lines = content.split('\n');

    for (int lineNum = 0; lineNum < lines.size(); ++lineNum) {
        const QString &line = lines[lineNum];

        for (int col = 0; col < line.length(); ++col) {
            // Skip if in comment
            if (isInComment(line, col)) {
                break;
            }

            QChar ch = line[col];

            // Skip escaped braces
            if (col > 0 && line[col-1] == '\\') {
                continue;
            }

            if (ch == '{') {
                braceStack.push({lineNum, col, '{'});
            } else if (ch == '}') {
                if (braceStack.isEmpty()) {
                    errors.append(LaTeXError(
                        LaTeXError::UnmatchedBrace,
                        lineNum,
                        col,
                        "Unmatched closing brace '}'",
                        "}"
                    ));
                } else {
                    braceStack.pop();
                }
            } else if (ch == '[') {
                bracketStack.push({lineNum, col, '['});
            } else if (ch == ']') {
                if (!bracketStack.isEmpty()) {
                    bracketStack.pop();
                }
                // Don't report unmatched ] as error since they're often optional
            }
        }
    }

    // Report unclosed braces
    while (!braceStack.isEmpty()) {
        BraceInfo info = braceStack.pop();
        errors.append(LaTeXError(
            LaTeXError::UnmatchedBrace,
            info.line,
            info.column,
            "Unclosed brace '{'",
            "{"
        ));
    }

    // Report unclosed brackets
    while (!bracketStack.isEmpty()) {
        BraceInfo info = bracketStack.pop();
        errors.append(LaTeXError(
            LaTeXError::UnmatchedBracket,
            info.line,
            info.column,
            "Unclosed bracket '['",
            "["
        ));
    }

    return errors;
}

QVector<LaTeXError> LaTeXErrorChecker::checkEnvironments(const QString &content) {
    QVector<LaTeXError> errors;
    QStack<EnvironmentInfo> envStack;

    QRegularExpression beginRegex(R"(\\begin\{([^}]+)\})");
    QRegularExpression endRegex(R"(\\end\{([^}]+)\})");

    QStringList lines = content.split('\n');

    for (int lineNum = 0; lineNum < lines.size(); ++lineNum) {
        const QString &line = lines[lineNum];

        // Check for \begin
        QRegularExpressionMatchIterator beginIt = beginRegex.globalMatch(line);
        while (beginIt.hasNext()) {
            QRegularExpressionMatch match = beginIt.next();
            if (!isInComment(line, match.capturedStart())) {
                QString envName = match.captured(1);
                envStack.push({envName, lineNum, static_cast<int>(match.capturedStart())});
            }
        }

        // Check for \end
        QRegularExpressionMatchIterator endIt = endRegex.globalMatch(line);
        while (endIt.hasNext()) {
            QRegularExpressionMatch match = endIt.next();
            if (!isInComment(line, match.capturedStart())) {
                QString envName = match.captured(1);

                if (envStack.isEmpty()) {
                    errors.append(LaTeXError(
                        LaTeXError::UnmatchedEnvironment,
                        lineNum,
                        static_cast<int>(match.capturedStart()),
                        QString("Unmatched \\end{%1}").arg(envName),
                        match.captured(0)
                    ));
                } else {
                    EnvironmentInfo info = envStack.pop();
                    if (info.name != envName) {
                        errors.append(LaTeXError(
                            LaTeXError::UnmatchedEnvironment,
                            lineNum,
                            static_cast<int>(match.capturedStart()),
                            QString("Environment mismatch: expected \\end{%1} but found \\end{%2}")
                                .arg(info.name, envName),
                            match.captured(0)
                        ));
                        // Push it back since it wasn't the right match
                        envStack.push(info);
                    }
                }
            }
        }
    }

    // Report unclosed environments
    while (!envStack.isEmpty()) {
        EnvironmentInfo info = envStack.pop();
        errors.append(LaTeXError(
            LaTeXError::UnmatchedEnvironment,
            info.line,
            info.column,
            QString("Unclosed environment: \\begin{%1}").arg(info.name),
            QString("\\begin{%1}").arg(info.name)
        ));
    }

    return errors;
}

QVector<LaTeXError> LaTeXErrorChecker::checkMathDelimiters(const QString &content) {
    QVector<LaTeXError> errors;

    QStringList lines = content.split('\n');

    for (int lineNum = 0; lineNum < lines.size(); ++lineNum) {
        const QString &line = lines[lineNum];

        int dollarCount = 0;
        int doubleDollarCount = 0;

        for (int col = 0; col < line.length(); ++col) {
            if (isInComment(line, col)) {
                break;
            }

            // Skip escaped $
            if (col > 0 && line[col-1] == '\\') {
                continue;
            }

            if (line[col] == '$') {
                // Check for $$
                if (col + 1 < line.length() && line[col + 1] == '$') {
                    doubleDollarCount++;
                    col++; // Skip next $
                } else {
                    dollarCount++;
                }
            }
        }

        // Single $ should appear in pairs on same line
        if (dollarCount % 2 != 0) {
            errors.append(LaTeXError(
                LaTeXError::UnmatchedMathDelimiter,
                lineNum,
                0,
                "Unmatched math delimiter '$' (inline math must be closed on same line)",
                line
            ));
        }
    }

    // Check for \( \) and \[ \] delimiters
    QRegularExpression mathOpenParen(R"(\\\()");
    QRegularExpression mathCloseParen(R"(\\\))");
    QRegularExpression mathOpenBracket(R"(\\\[)");
    QRegularExpression mathCloseBracket(R"(\\\])");

    int parenBalance = 0;
    int bracketBalance = 0;

    for (int lineNum = 0; lineNum < lines.size(); ++lineNum) {
        const QString &line = lines[lineNum];

        parenBalance += line.count(mathOpenParen);
        parenBalance -= line.count(mathCloseParen);

        bracketBalance += line.count(mathOpenBracket);
        bracketBalance -= line.count(mathCloseBracket);
    }

    if (parenBalance != 0) {
        errors.append(LaTeXError(
            LaTeXError::UnmatchedMathDelimiter,
            0,
            0,
            QString("Unmatched math delimiters \\( \\): %1 %2")
                .arg(parenBalance > 0 ? "unclosed" : "extra closing")
                .arg(qAbs(parenBalance)),
            ""
        ));
    }

    if (bracketBalance != 0) {
        errors.append(LaTeXError(
            LaTeXError::UnmatchedMathDelimiter,
            0,
            0,
            QString("Unmatched math delimiters \\[ \\]: %1 %2")
                .arg(bracketBalance > 0 ? "unclosed" : "extra closing")
                .arg(qAbs(bracketBalance)),
            ""
        ));
    }

    return errors;
}
