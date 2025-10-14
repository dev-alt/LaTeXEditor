#include "DocumentModel.h"
#include <QProcess>
#include <QTemporaryFile>
#include <QTextStream>
#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>

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

QString DocumentModel::getCurrentFilePath() const {
    return m_currentFilePath;
}

void DocumentModel::setCurrentFilePath(const QString &filePath) {
    m_currentFilePath = filePath;
}

QString DocumentModel::generateAutoSaveFilePath() const {
    if (m_currentFilePath.isEmpty()) {
        // For untitled documents, save in temp directory
        QString tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
        return QDir(tempDir).filePath("LaTeXEditor_untitled_autosave.tex");
    } else {
        // Save alongside the current file with .autosave extension
        QFileInfo fileInfo(m_currentFilePath);
        QString dir = fileInfo.absolutePath();
        QString baseName = fileInfo.baseName();
        QString extension = fileInfo.completeSuffix();
        return QDir(dir).filePath(QString(".%1.%2.autosave").arg(baseName).arg(extension));
    }
}

QString DocumentModel::getAutoSaveFilePath() const {
    return generateAutoSaveFilePath();
}

bool DocumentModel::hasAutoSaveFile() const {
    return QFileInfo::exists(generateAutoSaveFilePath());
}