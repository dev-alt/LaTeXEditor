// DocumentModel.h
#ifndef DOCUMENTMODEL_H
#define DOCUMENTMODEL_H

#include <QString>
#include <QObject>

class DocumentModel : public QObject {
Q_OBJECT

public:
    explicit DocumentModel(QObject *parent = nullptr);

    QString getContent() const;
    void setContent(const QString &content);
    void clear();
    bool isModified() const;
    void setModified(bool modified);

    QString generatePreview() const;

    // File path management
    QString getCurrentFilePath() const;
    void setCurrentFilePath(const QString &filePath);

    // Auto-save support
    QString getAutoSaveFilePath() const;
    bool hasAutoSaveFile() const;

signals:
    void contentChanged();

private:
    QString m_content;
    bool m_modified;
    QString m_currentFilePath;

    QString generateAutoSaveFilePath() const;
};

#endif // DOCUMENTMODEL_H