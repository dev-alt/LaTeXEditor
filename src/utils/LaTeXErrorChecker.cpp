#include "LaTeXErrorChecker.h"
#include <QRegularExpression>
#include <QStack>
#include <QDebug>

LaTeXErrorChecker::LaTeXErrorChecker(QObject *parent) : QObject(parent) {
    initializeCommandDatabase();
    initializePackageDatabase();
}

QVector<LaTeXError> LaTeXErrorChecker::checkDocument(const QString &content) {
    QVector<LaTeXError> errors;

    // Check braces and brackets
    errors.append(checkBraces(content));

    // Check environments
    errors.append(checkEnvironments(content));

    // Check math delimiters
    errors.append(checkMathDelimiters(content));

    // Check commands (semantic validation)
    errors.append(checkCommands(content));

    // Check packages
    errors.append(checkPackages(content));

    // Check common mistakes
    errors.append(checkCommonMistakes(content));

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

void LaTeXErrorChecker::initializeCommandDatabase() {
    // Math mode commands
    m_mathCommands = {
        "frac", "sqrt", "sum", "int", "prod", "lim",
        "alpha", "beta", "gamma", "delta", "epsilon",
        "theta", "lambda", "mu", "pi", "sigma", "omega",
        "infty", "partial", "nabla", "forall", "exists",
        "leq", "geq", "neq", "approx", "equiv",
        "times", "cdot", "pm", "mp"
    };

    // Deprecated commands
    m_deprecatedCommands = {
        "bf", "it", "rm", "sf", "tt", "sc",
        "over", "atop", "above", "choose"
    };

    // Package-specific commands
    m_packageCommands = {
        {"includegraphics", "graphicx"},
        {"url", "url"},
        {"href", "hyperref"},
        {"cite", "natbib"},
        {"citep", "natbib"},
        {"citet", "natbib"},
        {"textcolor", "xcolor"},
        {"definecolor", "xcolor"},
        {"listings", "listings"},
        {"lstlisting", "listings"}
    };
}

void LaTeXErrorChecker::initializePackageDatabase() {
    // This is already partially done in m_packageCommands
    // Could be extended with more detailed package information
}

QVector<LaTeXError> LaTeXErrorChecker::checkCommands(const QString &content) {
    QVector<LaTeXError> errors;

    QRegularExpression cmdRegex(R"(\\([a-zA-Z]+))");
    QStringList lines = content.split('\n');

    // Track if we have loaded packages
    QSet<QString> loadedPackages;
    QRegularExpression usepackageRegex(R"(\\usepackage(?:\[[^\]]*\])?\{([^}]+)\})");

    // First pass: collect loaded packages
    for (const QString &line : lines) {
        QRegularExpressionMatchIterator it = usepackageRegex.globalMatch(line);
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            QString packages = match.captured(1);
            for (const QString &pkg : packages.split(',')) {
                loadedPackages.insert(pkg.trimmed());
            }
        }
    }

    // Second pass: check commands
    for (int lineNum = 0; lineNum < lines.size(); ++lineNum) {
        const QString &line = lines[lineNum];

        QRegularExpressionMatchIterator it = cmdRegex.globalMatch(line);
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();

            if (isInComment(line, match.capturedStart())) {
                continue;
            }

            QString cmdName = match.captured(1);

            // Check if command requires a package
            if (m_packageCommands.contains(cmdName)) {
                QString requiredPackage = m_packageCommands[cmdName];
                if (!loadedPackages.contains(requiredPackage)) {
                    errors.append(LaTeXError(
                        LaTeXError::MissingPackage,
                        lineNum,
                        match.capturedStart(),
                        QString("Command \\%1 requires package '%2'").arg(cmdName, requiredPackage),
                        match.captured(0)
                    ));
                }
            }

            // Check deprecated commands
            if (m_deprecatedCommands.contains(cmdName)) {
                QString replacement;
                if (cmdName == "bf") replacement = "\\textbf{} or \\bfseries";
                else if (cmdName == "it") replacement = "\\textit{} or \\itshape";
                else if (cmdName == "rm") replacement = "\\textrm{} or \\rmfamily";
                else if (cmdName == "tt") replacement = "\\texttt{} or \\ttfamily";
                else if (cmdName == "sc") replacement = "\\textsc{} or \\scshape";
                else if (cmdName == "sf") replacement = "\\textsf{} or \\sffamily";
                else replacement = "(see LaTeX documentation)";

                errors.append(LaTeXError(
                    LaTeXError::DeprecatedCommand,
                    lineNum,
                    match.capturedStart(),
                    QString("Deprecated command \\%1, use %2 instead").arg(cmdName, replacement),
                    match.captured(0)
                ));
            }
        }
    }

    return errors;
}

QVector<LaTeXError> LaTeXErrorChecker::checkPackages(const QString &content) {
    QVector<LaTeXError> errors;

    QStringList lines = content.split('\n');
    bool foundBeginDocument = false;
    int beginDocumentLine = -1;

    for (int lineNum = 0; lineNum < lines.size(); ++lineNum) {
        const QString &line = lines[lineNum];

        if (line.contains(QRegularExpression(R"(\\begin\{document\})"))) {
            foundBeginDocument = true;
            beginDocumentLine = lineNum;
        }

        // Check for \usepackage after \begin{document}
        if (foundBeginDocument && line.contains(QRegularExpression(R"(\\usepackage)"))) {
            if (!isInComment(line, line.indexOf("\\usepackage"))) {
                errors.append(LaTeXError(
                    LaTeXError::UsePackageAfterBeginDocument,
                    lineNum,
                    line.indexOf("\\usepackage"),
                    QString("\\usepackage must be used before \\begin{document} (line %1)").arg(beginDocumentLine + 1),
                    line.trimmed()
                ));
            }
        }
    }

    return errors;
}

QVector<LaTeXError> LaTeXErrorChecker::checkCommonMistakes(const QString &content) {
    QVector<LaTeXError> errors;

    QStringList lines = content.split('\n');

    for (int lineNum = 0; lineNum < lines.size(); ++lineNum) {
        const QString &line = lines[lineNum];

        // Check for common spacing mistakes
        if (line.contains(QRegularExpression(R"(\w\\\w)"))) {
            // Missing space after backslash command
            QRegularExpression pattern(R"(\w(\\[a-zA-Z]+)\w)");
            QRegularExpressionMatchIterator it = pattern.globalMatch(line);
            while (it.hasNext()) {
                QRegularExpressionMatch match = it.next();
                if (!isInComment(line, match.capturedStart())) {
                    errors.append(LaTeXError(
                        LaTeXError::InvalidCommand,
                        lineNum,
                        match.capturedStart(),
                        QString("Missing space or {} after command %1").arg(match.captured(1)),
                        match.captured(0)
                    ));
                }
            }
        }

        // Check for double spaces (common typo)
        if (line.contains("  ") && !isInComment(line, line.indexOf("  "))) {
            int pos = line.indexOf("  ");
            // Only warn if not in verbatim-like content
            if (!line.contains("\\verb") && !line.trimmed().startsWith("%")) {
                errors.append(LaTeXError(
                    LaTeXError::InvalidCommand,
                    lineNum,
                    pos,
                    "Multiple consecutive spaces (LaTeX ignores extra spaces, but this may be unintentional)",
                    "  "
                ));
            }
        }

        // Check for missing $ in common math expressions
        QRegularExpression mathPattern(R"(\b(?:x|y|z|n|i|j|k)\s*[=<>]\s*\d+\b)");
        if (!line.contains('$') && mathPattern.match(line).hasMatch()) {
            QRegularExpressionMatch match = mathPattern.match(line);
            if (!isInComment(line, match.capturedStart())) {
                errors.append(LaTeXError(
                    LaTeXError::MathModeRequired,
                    lineNum,
                    match.capturedStart(),
                    "Mathematical expression should be in math mode ($...$)",
                    match.captured(0)
                ));
            }
        }

        // Check for \\ outside of tables/arrays
        if (line.contains(R"(\\)") && !line.contains("\\begin{") && !line.contains("\\end{")) {
            // Check if we're likely in a table environment
            bool likelyInTable = line.contains("&");
            if (!likelyInTable && !isInComment(line, line.indexOf(R"(\\)"))) {
                int pos = line.indexOf(R"(\\)");
                errors.append(LaTeXError(
                    LaTeXError::InvalidCommand,
                    lineNum,
                    pos,
                    "Use of \\\\ outside table/array environment (use \\par or blank line for paragraphs)",
                    R"(\\)"
                ));
            }
        }
    }

    return errors;
}
