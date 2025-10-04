#include "PreviewController.h"
#include <QDebug>
#include <QRegularExpression>

PreviewController::PreviewController(DocumentModel *model, PreviewWindow *view, QObject *parent)
        : QObject(parent), m_model(model), m_view(view) {

    // Create timer for debounced updates
    m_updateTimer = new QTimer(this);
    m_updateTimer->setSingleShot(true);
    m_updateTimer->setInterval(500); // 500ms delay
    connect(m_updateTimer, &QTimer::timeout, this, [this]() { updatePreview(); });

    // Connect model changes to schedule update
    connect(m_model, &DocumentModel::contentChanged, this, &PreviewController::schedulePreviewUpdate);

    // Initial preview
    updatePreview();
}

void PreviewController::updatePreview(const QString &content) {
    qDebug() << "PreviewController::updatePreview called";
    QString latexContent = content.isEmpty() ? m_model->getContent() : content;
    QString htmlContent = generatePreviewContent(latexContent);
    m_view->updatePreview(htmlContent);
    qDebug() << "Preview updated";
}

void PreviewController::schedulePreviewUpdate() {
    // Restart the timer - this debounces rapid changes
    m_updateTimer->start();
}

QString PreviewController::generatePreviewContent(const QString &latexContent) {
    // Convert LaTeX content to HTML with MathJax rendering
    QString processedContent = latexContent;

    // Replace LaTeX environments with HTML equivalents
    processedContent.replace("\\begin{document}", "");
    processedContent.replace("\\end{document}", "");
    processedContent.replace("\\maketitle", "");

    // Convert common LaTeX commands to HTML
    processedContent.replace(QRegularExpression("\\\\section\\{([^}]+)\\}"), "<h2>\\1</h2>");
    processedContent.replace(QRegularExpression("\\\\subsection\\{([^}]+)\\}"), "<h3>\\1</h3>");
    processedContent.replace(QRegularExpression("\\\\subsubsection\\{([^}]+)\\}"), "<h4>\\1</h4>");
    processedContent.replace(QRegularExpression("\\\\title\\{([^}]+)\\}"), "<h1>\\1</h1>");
    processedContent.replace(QRegularExpression("\\\\author\\{([^}]+)\\}"), "<p class='author'>\\1</p>");
    processedContent.replace(QRegularExpression("\\\\date\\{([^}]+)\\}"), "<p class='date'>\\1</p>");

    // Convert text formatting
    processedContent.replace(QRegularExpression("\\\\textbf\\{([^}]+)\\}"), "<strong>\\1</strong>");
    processedContent.replace(QRegularExpression("\\\\textit\\{([^}]+)\\}"), "<em>\\1</em>");
    processedContent.replace(QRegularExpression("\\\\emph\\{([^}]+)\\}"), "<em>\\1</em>");
    processedContent.replace(QRegularExpression("\\\\texttt\\{([^}]+)\\}"), "<code>\\1</code>");

    // Convert lists
    processedContent.replace("\\begin{itemize}", "<ul>");
    processedContent.replace("\\end{itemize}", "</ul>");
    processedContent.replace("\\begin{enumerate}", "<ol>");
    processedContent.replace("\\end{enumerate}", "</ol>");
    processedContent.replace(QRegularExpression("\\\\item\\s+"), "<li>");

    // Keep math environments as-is for MathJax
    // Inline math: \(...\) or $...$
    // Display math: \[...\] or $$...$$

    // Build HTML with MathJax
    QString htmlContent = R"(
<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>LaTeX Preview</title>
    <script src="https://polyfill.io/v3/polyfill.min.js?features=es6"></script>
    <script id="MathJax-script" async src="https://cdn.jsdelivr.net/npm/mathjax@3/es5/tex-mml-chtml.js"></script>
    <script>
        MathJax = {
            tex: {
                inlineMath: [['$', '$'], ['\\(', '\\)']],
                displayMath: [['$$', '$$'], ['\\[', '\\]']],
                processEscapes: true,
                processEnvironments: true
            },
            options: {
                skipHtmlTags: ['script', 'noscript', 'style', 'textarea', 'pre']
            }
        };
    </script>
    <style>
        body {
            font-family: 'Computer Modern', 'Latin Modern', serif;
            max-width: 800px;
            margin: 20px auto;
            padding: 20px;
            line-height: 1.6;
            background: white;
            color: #333;
        }
        h1 { font-size: 2em; margin-top: 0.67em; margin-bottom: 0.67em; }
        h2 { font-size: 1.5em; margin-top: 0.83em; margin-bottom: 0.83em; border-bottom: 1px solid #ccc; }
        h3 { font-size: 1.17em; margin-top: 1em; margin-bottom: 1em; }
        h4 { font-size: 1em; margin-top: 1.33em; margin-bottom: 1.33em; }
        .author { text-align: center; font-size: 1.2em; margin: 0.5em 0; }
        .date { text-align: center; color: #666; margin: 0.5em 0; }
        code { background: #f4f4f4; padding: 2px 6px; border-radius: 3px; font-family: monospace; }
        pre { background: #f4f4f4; padding: 10px; border-radius: 5px; overflow-x: auto; }
        ul, ol { margin: 1em 0; padding-left: 2em; }
        li { margin: 0.5em 0; }
        .mjx-math { font-size: 1.1em; }
    </style>
</head>
<body>
)" + processedContent + R"(
</body>
</html>
)";

    return htmlContent;
}