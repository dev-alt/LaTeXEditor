// ProjectModel.cpp
#include "ProjectModel.h"
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>

ProjectModel::ProjectModel(QObject *parent)
    : QObject(parent)
{
}

void ProjectModel::setMainFile(const QString &filePath) {
    if (m_mainFile != filePath) {
        m_mainFile = filePath;
        m_projectFiles.clear();

        if (!filePath.isEmpty()) {
            addFileToProject(filePath, true);
            scanProjectFiles();
        }

        emit mainFileChanged(filePath);
        emit projectChanged();
    }
}

QString ProjectModel::getMainFile() const {
    return m_mainFile;
}

bool ProjectModel::hasProject() const {
    return !m_mainFile.isEmpty();
}

void ProjectModel::closeProject() {
    m_mainFile.clear();
    m_projectFiles.clear();
    emit projectChanged();
}

QString ProjectModel::getProjectDirectory() const {
    if (m_mainFile.isEmpty()) {
        return QString();
    }
    return QFileInfo(m_mainFile).absolutePath();
}

QStringList ProjectModel::getAllFiles() const {
    QStringList files;
    for (const ProjectFile &file : m_projectFiles) {
        files.append(file.filePath);
    }
    return files;
}

ProjectFile ProjectModel::getFileInfo(const QString &filePath) const {
    for (const ProjectFile &file : m_projectFiles) {
        if (file.filePath == filePath) {
            return file;
        }
    }
    return ProjectFile();
}

bool ProjectModel::isFileInProject(const QString &filePath) const {
    for (const ProjectFile &file : m_projectFiles) {
        if (file.filePath == filePath) {
            return true;
        }
    }
    return false;
}

void ProjectModel::scanProjectFiles() {
    if (m_mainFile.isEmpty()) {
        return;
    }

    qDebug() << "Scanning project files starting from:" << m_mainFile;

    // Use a set to avoid duplicates during scanning
    QSet<QString> processedFiles;
    QList<QString> filesToProcess;
    filesToProcess.append(m_mainFile);

    while (!filesToProcess.isEmpty()) {
        QString currentFile = filesToProcess.takeFirst();

        if (processedFiles.contains(currentFile)) {
            continue;
        }

        processedFiles.insert(currentFile);

        // Parse includes from this file
        QStringList includes = parseIncludesFromFile(currentFile);

        // Add includes to project
        for (const QString &includePath : includes) {
            QString resolvedPath = resolveIncludePath(currentFile, includePath);
            if (!resolvedPath.isEmpty() && !processedFiles.contains(resolvedPath)) {
                addFileToProject(resolvedPath, false);
                filesToProcess.append(resolvedPath);
            }
        }

        // Update the ProjectFile with its includes
        for (ProjectFile &file : m_projectFiles) {
            if (file.filePath == currentFile) {
                file.includedFiles = includes;
                break;
            }
        }
    }

    qDebug() << "Project scan complete. Found" << m_projectFiles.size() << "files";
    emit filesScanned();
}

QStringList ProjectModel::findIncludedFiles(const QString &filePath) const {
    return parseIncludesFromFile(filePath);
}

QList<ProjectFile> ProjectModel::getProjectFiles() const {
    return m_projectFiles;
}

void ProjectModel::addFileToProject(const QString &filePath, bool isMain) {
    if (!isFileInProject(filePath)) {
        ProjectFile file(filePath, isMain);
        m_projectFiles.append(file);
        qDebug() << "Added file to project:" << filePath;
    }
}

QStringList ProjectModel::parseIncludesFromFile(const QString &filePath) const {
    QStringList includes;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for parsing:" << filePath;
        return includes;
    }

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    // Regular expressions for \include{}, \input{}, and \subfile{}
    // Match both with and without .tex extension
    QRegularExpression includeRegex(R"(\\(?:include|input|subfile)\{([^}]+)\})");
    QRegularExpressionMatchIterator it = includeRegex.globalMatch(content);

    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        QString includePath = match.captured(1);

        // Add .tex extension if not present
        if (!includePath.endsWith(".tex")) {
            includePath += ".tex";
        }

        includes.append(includePath);
    }

    return includes;
}

QString ProjectModel::resolveIncludePath(const QString &basePath, const QString &includePath) const {
    QFileInfo baseInfo(basePath);
    QString baseDir = baseInfo.absolutePath();

    // First try: relative to the including file's directory
    QString fullPath = QDir(baseDir).filePath(includePath);
    if (QFile::exists(fullPath)) {
        return QFileInfo(fullPath).absoluteFilePath();
    }

    // Second try: relative to project root
    if (hasProject()) {
        QString projectDir = getProjectDirectory();
        fullPath = QDir(projectDir).filePath(includePath);
        if (QFile::exists(fullPath)) {
            return QFileInfo(fullPath).absoluteFilePath();
        }
    }

    // Third try: check common subdirectories
    QStringList subdirs = {"chapters", "sections", "content", "includes"};
    for (const QString &subdir : subdirs) {
        fullPath = QDir(baseDir).filePath(subdir + "/" + includePath);
        if (QFile::exists(fullPath)) {
            return QFileInfo(fullPath).absoluteFilePath();
        }
    }

    qWarning() << "Could not resolve include path:" << includePath << "from" << basePath;
    return QString();
}
