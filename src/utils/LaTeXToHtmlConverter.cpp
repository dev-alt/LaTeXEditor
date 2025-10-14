// LaTeXToHtmlConverter.cpp
#include "LaTeXToHtmlConverter.h"
#include <QDebug>
#include <QCoreApplication>

LaTeXToHtmlConverter::LaTeXToHtmlConverter() {
    initializeSymbolMap();
    initializeEnvironmentMap();
}

void LaTeXToHtmlConverter::initializeSymbolMap() {
    // Common LaTeX special characters
    m_symbolMap["---"] = "&mdash;";  // em dash
    m_symbolMap["--"] = "&ndash;";   // en dash
    m_symbolMap["``"] = "&ldquo;";   // left double quote
    m_symbolMap["''"] = "&rdquo;";   // right double quote
    m_symbolMap["`"] = "&lsquo;";    // left single quote (when alone)
    m_symbolMap["'"] = "&rsquo;";    // right single quote (when alone)
    m_symbolMap["~"] = "&nbsp;";     // non-breaking space
}

void LaTeXToHtmlConverter::initializeEnvironmentMap() {
    m_environmentMap["abstract"] = "div class='abstract'";
    m_environmentMap["quote"] = "blockquote";
    m_environmentMap["quotation"] = "blockquote";
    m_environmentMap["center"] = "div style='text-align: center;'";
    m_environmentMap["flushleft"] = "div style='text-align: left;'";
    m_environmentMap["flushright"] = "div style='text-align: right;'";
    m_environmentMap["verbatim"] = "pre";
}

QString LaTeXToHtmlConverter::convertToHtml(const QString &latexContent, bool useCdn) {
    QString processed = latexContent;

    // Process in order of complexity
    processed = processDocumentStructure(processed);
    processed = processVerbatim(processed);
    processed = processSections(processed);
    processed = processTextFormatting(processed);
    processed = processLists(processed);
    processed = processTables(processed);
    processed = processEnvironments(processed);
    processed = processSpecialCharacters(processed);
    processed = processPackageCommands(processed);
    processed = processReferences(processed);
    processed = processMathEnvironments(processed);

    return generateHtmlDocument(processed, useCdn);
}

QString LaTeXToHtmlConverter::processDocumentStructure(const QString &content) {
    QString result = content;

    // Remove preamble commands
    result.replace(QRegularExpression("\\\\documentclass(\\[.*?\\])?\\{.*?\\}"), "");
    result.replace(QRegularExpression("\\\\usepackage(\\[.*?\\])?\\{.*?\\}"), "");
    result.replace("\\begin{document}", "");
    result.replace("\\end{document}", "");

    // Process title, author, date
    result.replace(QRegularExpression("\\\\title\\{([^}]+)\\}"), "<h1 class='title'>\\1</h1>");
    result.replace(QRegularExpression("\\\\author\\{([^}]+)\\}"), "<div class='author'>\\1</div>");
    result.replace(QRegularExpression("\\\\date\\{([^}]+)\\}"), "<div class='date'>\\1</div>");
    result.replace("\\maketitle", "");
    result.replace("\\tableofcontents", "<div class='toc'>[Table of Contents]</div>");

    return result;
}

QString LaTeXToHtmlConverter::processSections(const QString &content) {
    QString result = content;

    // Section hierarchy with numbering support
    result.replace(QRegularExpression("\\\\part\\{([^}]+)\\}"), "<h1 class='part'>\\1</h1>");
    result.replace(QRegularExpression("\\\\chapter\\{([^}]+)\\}"), "<h1 class='chapter'>\\1</h1>");
    result.replace(QRegularExpression("\\\\section\\{([^}]+)\\}"), "<h2>\\1</h2>");
    result.replace(QRegularExpression("\\\\subsection\\{([^}]+)\\}"), "<h3>\\1</h3>");
    result.replace(QRegularExpression("\\\\subsubsection\\{([^}]+)\\}"), "<h4>\\1</h4>");
    result.replace(QRegularExpression("\\\\paragraph\\{([^}]+)\\}"), "<h5>\\1</h5>");
    result.replace(QRegularExpression("\\\\subparagraph\\{([^}]+)\\}"), "<h6>\\1</h6>");

    // Starred versions (no numbering)
    result.replace(QRegularExpression("\\\\section\\*\\{([^}]+)\\}"), "<h2>\\1</h2>");
    result.replace(QRegularExpression("\\\\subsection\\*\\{([^}]+)\\}"), "<h3>\\1</h3>");
    result.replace(QRegularExpression("\\\\subsubsection\\*\\{([^}]+)\\}"), "<h4>\\1</h4>");

    return result;
}

QString LaTeXToHtmlConverter::processTextFormatting(const QString &content) {
    QString result = content;

    // Font styles
    result.replace(QRegularExpression("\\\\textbf\\{([^}]+)\\}"), "<strong>\\1</strong>");
    result.replace(QRegularExpression("\\\\textit\\{([^}]+)\\}"), "<em>\\1</em>");
    result.replace(QRegularExpression("\\\\emph\\{([^}]+)\\}"), "<em>\\1</em>");
    result.replace(QRegularExpression("\\\\texttt\\{([^}]+)\\}"), "<code>\\1</code>");
    result.replace(QRegularExpression("\\\\textsc\\{([^}]+)\\}"), "<span style='font-variant: small-caps;'>\\1</span>");
    result.replace(QRegularExpression("\\\\underline\\{([^}]+)\\}"), "<u>\\1</u>");

    // Font sizes
    result.replace(QRegularExpression("\\\\tiny\\{([^}]+)\\}"), "<span style='font-size: 0.6em;'>\\1</span>");
    result.replace(QRegularExpression("\\\\small\\{([^}]+)\\}"), "<span style='font-size: 0.9em;'>\\1</span>");
    result.replace(QRegularExpression("\\\\large\\{([^}]+)\\}"), "<span style='font-size: 1.2em;'>\\1</span>");
    result.replace(QRegularExpression("\\\\Large\\{([^}]+)\\}"), "<span style='font-size: 1.5em;'>\\1</span>");
    result.replace(QRegularExpression("\\\\LARGE\\{([^}]+)\\}"), "<span style='font-size: 1.8em;'>\\1</span>");
    result.replace(QRegularExpression("\\\\huge\\{([^}]+)\\}"), "<span style='font-size: 2em;'>\\1</span>");

    // Colors (basic support)
    result.replace(QRegularExpression("\\\\textcolor\\{red\\}\\{([^}]+)\\}"), "<span style='color: red;'>\\1</span>");
    result.replace(QRegularExpression("\\\\textcolor\\{blue\\}\\{([^}]+)\\}"), "<span style='color: blue;'>\\1</span>");
    result.replace(QRegularExpression("\\\\textcolor\\{green\\}\\{([^}]+)\\}"), "<span style='color: green;'>\\1</span>");

    // Spacing commands
    result.replace("\\\\", "<br>");  // Line break
    result.replace("\\par", "<p>");   // Paragraph break
    result.replace("\\newpage", "<hr style='page-break-after: always;'>");
    result.replace("\\clearpage", "<hr style='page-break-after: always;'>");

    return result;
}

QString LaTeXToHtmlConverter::processLists(const QString &content) {
    QString result = content;

    // Lists
    result.replace("\\begin{itemize}", "<ul>");
    result.replace("\\end{itemize}", "</ul>");
    result.replace("\\begin{enumerate}", "<ol>");
    result.replace("\\end{enumerate}", "</ol>");
    result.replace("\\begin{description}", "<dl>");
    result.replace("\\end{description}", "</dl>");

    // List items
    result.replace(QRegularExpression("\\\\item\\s+"), "<li>");
    result.replace(QRegularExpression("\\\\item\\[([^\\]]+)\\]\\s*"), "<dt>\\1</dt><dd>");

    return result;
}

QString LaTeXToHtmlConverter::processTables(const QString &content) {
    QString result = content;

    // Basic table support
    QRegularExpression tableRegex("\\\\begin\\{tabular\\}(\\{[^}]+\\})([\\s\\S]*?)\\\\end\\{tabular\\}");
    QRegularExpressionMatchIterator it = tableRegex.globalMatch(content);

    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        QString tableContent = match.captured(2);

        // Convert rows
        QStringList rows = tableContent.split("\\\\");
        QString htmlTable = "<table border='1' cellpadding='5' cellspacing='0'>";

        for (const QString &row : rows) {
            if (row.trimmed().isEmpty()) continue;

            QString cleanRow = row.trimmed();
            cleanRow.replace("\\hline", "");  // Remove hlines

            QStringList cells = cleanRow.split("&");
            htmlTable += "<tr>";
            for (const QString &cell : cells) {
                htmlTable += "<td>" + cell.trimmed() + "</td>";
            }
            htmlTable += "</tr>";
        }

        htmlTable += "</table>";
        result.replace(match.captured(0), htmlTable);
    }

    // Table environment
    result.replace(QRegularExpression("\\\\begin\\{table\\}(\\[.*?\\])?"), "<div class='table'>");
    result.replace("\\end{table}", "</div>");
    result.replace(QRegularExpression("\\\\caption\\{([^}]+)\\}"), "<div class='caption'>\\1</div>");

    return result;
}

QString LaTeXToHtmlConverter::processEnvironments(const QString &content) {
    QString result = content;

    // Process custom environments
    for (auto it = m_environmentMap.begin(); it != m_environmentMap.end(); ++it) {
        QString env = it.key();
        QString html = it.value();

        result.replace("\\begin{" + env + "}", "<" + html + ">");
        result.replace("\\end{" + env + "}", "</" + html.split(' ').first() + ">");
    }

    // Theorem-like environments
    result.replace(QRegularExpression("\\\\begin\\{theorem\\}"), "<div class='theorem'><strong>Theorem.</strong> ");
    result.replace("\\end{theorem}", "</div>");
    result.replace(QRegularExpression("\\\\begin\\{lemma\\}"), "<div class='lemma'><strong>Lemma.</strong> ");
    result.replace("\\end{lemma}", "</div>");
    result.replace(QRegularExpression("\\\\begin\\{proof\\}"), "<div class='proof'><em>Proof.</em> ");
    result.replace("\\end{proof}", " ∎</div>");

    // Figure environment
    result.replace(QRegularExpression("\\\\begin\\{figure\\}(\\[.*?\\])?"), "<figure>");
    result.replace("\\end{figure}", "</figure>");
    result.replace(QRegularExpression("\\\\includegraphics(\\[.*?\\])?\\{([^}]+)\\}"),
                   "<img src='\\2' alt='\\2' style='max-width: 100%;'>");

    return result;
}

QString LaTeXToHtmlConverter::processSpecialCharacters(const QString &content) {
    QString result = content;

    // Process symbol map
    for (auto it = m_symbolMap.begin(); it != m_symbolMap.end(); ++it) {
        result.replace(it.key(), it.value());
    }

    // LaTeX special characters
    result.replace("\\&", "&amp;");
    result.replace("\\%", "%");
    result.replace("\\$", "$");
    result.replace("\\#", "#");
    result.replace("\\_", "_");
    result.replace("\\{", "{");
    result.replace("\\}", "}");

    // Quotes
    result.replace("\\textquotedblleft", "&ldquo;");
    result.replace("\\textquotedblright", "&rdquo;");

    return result;
}

QString LaTeXToHtmlConverter::processPackageCommands(const QString &content) {
    QString result = content;

    // Hyperref package
    result.replace(QRegularExpression("\\\\href\\{([^}]+)\\}\\{([^}]+)\\}"),
                   "<a href='\\1' target='_blank'>\\2</a>");
    result.replace(QRegularExpression("\\\\url\\{([^}]+)\\}"),
                   "<a href='\\1' target='_blank'>\\1</a>");

    return result;
}

QString LaTeXToHtmlConverter::processReferences(const QString &content) {
    QString result = content;

    // Citations
    result.replace(QRegularExpression("\\\\cite\\{([^}]+)\\}"), "[<a href='#ref-\\1'>\\1</a>]");
    result.replace(QRegularExpression("\\\\ref\\{([^}]+)\\}"), "<a href='#\\1'>?</a>");
    result.replace(QRegularExpression("\\\\label\\{([^}]+)\\}"), "<a name='\\1'></a>");

    // Footnotes
    result.replace(QRegularExpression("\\\\footnote\\{([^}]+)\\}"),
                   "<sup><a href='#fn'>*</a></sup><span class='footnote'>\\1</span>");

    return result;
}

QString LaTeXToHtmlConverter::processMathEnvironments(const QString &content) {
    // Math environments are left as-is for MathJax to process
    // Just ensure they're properly formatted
    QString result = content;

    // Equation environments
    result.replace("\\begin{equation}", "\\[");
    result.replace("\\end{equation}", "\\]");
    result.replace("\\begin{equation*}", "\\[");
    result.replace("\\end{equation*}", "\\]");

    // Align environments (MathJax supports these)
    // Keep as-is

    return result;
}

QString LaTeXToHtmlConverter::processVerbatim(const QString &content) {
    QString result = content;

    // Verb command
    QRegularExpression verbRegex("\\\\verb\\|([^|]+)\\|");
    result.replace(verbRegex, "<code>\\1</code>");

    return result;
}

QString LaTeXToHtmlConverter::getMathJaxConfig(bool useCdn) {
    QString cdnScript = R"(
    <script src="https://polyfill.io/v3/polyfill.min.js?features=es6"></script>
    <script id="MathJax-script" async src="https://cdn.jsdelivr.net/npm/mathjax@3/es5/tex-mml-chtml.js"></script>
)";

    QString localScript = R"(
    <script>
        // MathJax would be loaded from local file if available
        // For now, fallback to CDN or basic rendering
    </script>
)";

    return QString(R"(
    <script>
        MathJax = {
            tex: {
                inlineMath: [['$', '$'], ['\\(', '\\)']],
                displayMath: [['$$', '$$'], ['\\[', '\\]']],
                processEscapes: true,
                processEnvironments: true,
                tags: 'ams',
                packages: {'[+]': ['ams', 'newcommand', 'configmacros']}
            },
            options: {
                skipHtmlTags: ['script', 'noscript', 'style', 'textarea', 'pre', 'code']
            },
            startup: {
                ready: () => {
                    MathJax.startup.defaultReady();
                    console.log('MathJax loaded');
                }
            }
        };
    </script>
)") + (useCdn ? cdnScript : localScript);
}

QString LaTeXToHtmlConverter::getStyles() {
    return R"(
        <style>
            body {
                font-family: 'Libertine', 'Linux Libertine', 'Georgia', 'Times New Roman', serif;
                max-width: 850px;
                margin: 20px auto;
                padding: 30px;
                line-height: 1.7;
                background: #ffffff;
                color: #2d2d2d;
                font-size: 16px;
            }

            /* Headings */
            h1, h2, h3, h4, h5, h6 {
                font-family: 'Libertine', 'Linux Libertine', 'Georgia', serif;
                font-weight: bold;
                margin-top: 1.5em;
                margin-bottom: 0.8em;
                color: #1a1a1a;
            }
            h1 { font-size: 2.2em; text-align: center; }
            h1.title { margin-bottom: 0.3em; }
            h2 { font-size: 1.75em; border-bottom: 2px solid #e0e0e0; padding-bottom: 0.3em; }
            h3 { font-size: 1.4em; }
            h4 { font-size: 1.2em; }
            h5 { font-size: 1.1em; }
            h6 { font-size: 1em; font-style: italic; }

            /* Title page elements */
            .title { margin-bottom: 0.5em; }
            .author {
                text-align: center;
                font-size: 1.3em;
                margin: 0.5em 0;
                font-style: italic;
            }
            .date {
                text-align: center;
                color: #666;
                margin: 0.5em 0 2em 0;
            }

            /* Text formatting */
            p { margin: 1em 0; text-align: justify; }
            strong { font-weight: bold; }
            em { font-style: italic; }
            code {
                background: #f5f5f5;
                padding: 2px 6px;
                border-radius: 3px;
                font-family: 'Courier New', Courier, monospace;
                font-size: 0.9em;
                border: 1px solid #e0e0e0;
            }
            pre {
                background: #f8f8f8;
                padding: 15px;
                border-radius: 5px;
                overflow-x: auto;
                border: 1px solid #ddd;
                line-height: 1.4;
            }

            /* Lists */
            ul, ol {
                margin: 1em 0;
                padding-left: 2.5em;
            }
            li {
                margin: 0.5em 0;
                line-height: 1.6;
            }
            dl { margin: 1em 0; }
            dt { font-weight: bold; margin-top: 0.5em; }
            dd { margin-left: 2em; margin-bottom: 0.5em; }

            /* Tables */
            table {
                border-collapse: collapse;
                margin: 1.5em auto;
                min-width: 50%;
            }
            td, th {
                border: 1px solid #ddd;
                padding: 8px 12px;
                text-align: left;
            }
            th {
                background-color: #f5f5f5;
                font-weight: bold;
            }
            .caption {
                text-align: center;
                font-size: 0.9em;
                font-style: italic;
                margin: 0.5em 0;
            }

            /* Environments */
            .abstract {
                margin: 2em auto;
                max-width: 90%;
                padding: 1em;
                background: #f9f9f9;
                border-left: 4px solid #4CAF50;
            }
            blockquote {
                margin: 1.5em 2em;
                padding: 1em;
                background: #f9f9f9;
                border-left: 4px solid #ccc;
                font-style: italic;
            }
            .theorem, .lemma, .proof {
                margin: 1.5em 0;
                padding: 1em;
                background: #f0f7ff;
                border-left: 4px solid #2196F3;
            }
            .proof {
                background: #f5f5f5;
                border-left: 4px solid #999;
            }

            /* Figures */
            figure {
                margin: 2em auto;
                text-align: center;
            }
            figure img {
                max-width: 100%;
                height: auto;
            }

            /* Links */
            a {
                color: #0066cc;
                text-decoration: none;
            }
            a:hover {
                text-decoration: underline;
            }

            /* Math */
            .mjx-math {
                font-size: 1.05em;
            }

            /* Footnotes */
            .footnote {
                font-size: 0.85em;
                color: #666;
                display: block;
                margin-top: 0.5em;
                padding-left: 1em;
            }
        </style>
    )";
}

QString LaTeXToHtmlConverter::generateHtmlDocument(const QString &body, bool useCdn) {
    return QString(R"(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>LaTeX Preview</title>
    %1
    %2
</head>
<body>
%3
</body>
</html>
)").arg(getMathJaxConfig(useCdn), getStyles(), body);
}

QString LaTeXToHtmlConverter::escapeHtml(const QString &text) {
    QString result = text;
    result.replace("&", "&amp;");
    result.replace("<", "&lt;");
    result.replace(">", "&gt;");
    result.replace("\"", "&quot;");
    result.replace("'", "&#39;");
    return result;
}

QString LaTeXToHtmlConverter::replaceCommand(const QString &content, const QString &command, const QString &htmlTag) {
    QString pattern = "\\\\" + command + "\\{([^}]+)\\}";
    QString replacement = "<" + htmlTag + ">\\1</" + htmlTag + ">";
    return QString(content).replace(QRegularExpression(pattern), replacement);
}
