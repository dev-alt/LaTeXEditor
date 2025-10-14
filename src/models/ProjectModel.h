// ProjectModel.h
#ifndef PROJECTMODEL_H
#define PROJECTMODEL_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QFileInfo>
#include <QDir>

// Represents a file in the LaTeX project
struct ProjectFile {
    QString filePath;           // Absolute path to the file
    QString displayName;        // Filename for display
    bool isMainFile;            // True if this is the main/root document
    QStringList includedFiles;  // Files referenced by \include{} or \input{}

    ProjectFile(const QString &path = QString(), bool main = false)
        : filePath(path)
        , displayName(QFileInfo(path).fileName())
        , isMainFile(main)
    {}
};

class ProjectModel : public QObject {
Q_OBJECT

public:
    explicit ProjectModel(QObject *parent = nullptr);

    // Project management
    void setMainFile(const QString &filePath);
    QString getMainFile() const;
    bool hasProject() const;
    void closeProject();

    // Get project directory
    QString getProjectDirectory() const;

    // File management
    QStringList getAllFiles() const;
    ProjectFile getFileInfo(const QString &filePath) const;
    bool isFileInProject(const QString &filePath) const;

    // Parse and discover included files
    void scanProjectFiles();
    QStringList findIncludedFiles(const QString &filePath) const;

    // Project files list
    QList<ProjectFile> getProjectFiles() const;

    // Helper to resolve include paths (exposed for UI)
    QString resolveIncludePath(const QString &basePath, const QString &includePath) const;

signals:
    void projectChanged();
    void mainFileChanged(const QString &filePath);
    void filesScanned();

private:
    QString m_mainFile;
    QList<ProjectFile> m_projectFiles;

    // Helper methods
    void addFileToProject(const QString &filePath, bool isMain = false);
    QStringList parseIncludesFromFile(const QString &filePath) const;
    QString resolveIncludePath(const QString &basePath, const QString &includePath) const;
};

#endif // PROJECTMODEL_H
