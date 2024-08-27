#include "DocumentModel.h"
#include <QProcess>
#include <QTemporaryFile>
#include <QTextStream>

DocumentModel::DocumentModel(QObject *parent) : QObject(parent), m_modified(false) {}

QString DocumentModel::getContent() const {
    return m_content;
}

void DocumentModel::setContent(const QString &content) {
    if (m_content != content) {
        m_content = content;
        m_modified = true;
        emit contentChanged();
    }
}

void DocumentModel::clear() {
    setContent("");
    m_modified = false;
}

bool DocumentModel::isModified() const {
    return m_modified;
}

void DocumentModel::setModified(bool modified) {
    m_modified = modified;
}

QString DocumentModel::generatePreview() const {
    QTemporaryFile tempFile;
    if (tempFile.open()) {
        QTextStream out(&tempFile);
        out << m_content;
        tempFile.close();

        QProcess latexProcess;
        latexProcess.start("pdflatex", QStringList() << "-interaction=nonstopmode" << tempFile.fileName());
        latexProcess.waitForFinished();

        if (latexProcess.exitCode() == 0) {
            // Convert PDF to HTML (you might need to implement this part)
            // For now, we'll just return the LaTeX content wrapped in pre tags
            return "<pre>" + m_content.toHtmlEscaped() + "</pre>";
        }
    }
    return "<p>Error generating preview</p>";
}