// AutoSaveController.h
#ifndef AUTOSAVECONTROLLER_H
#define AUTOSAVECONTROLLER_H

#include <QObject>
#include <QTimer>
#include <QString>

class DocumentModel;

class AutoSaveController : public QObject {
Q_OBJECT

public:
    explicit AutoSaveController(DocumentModel *model, QObject *parent = nullptr);
    ~AutoSaveController();

    // Enable/disable auto-save
    void setEnabled(bool enabled);
    bool isEnabled() const;

    // Set auto-save interval in milliseconds (default: 60000 = 1 minute)
    void setInterval(int milliseconds);
    int getInterval() const;

    // Manually trigger auto-save
    void saveNow();

    // Check for and recover from auto-save file
    bool hasRecoverableAutoSave() const;
    QString recoverAutoSave();
    void deleteAutoSaveFile();

signals:
    void autoSaved();
    void autoSaveFailed(const QString &error);
    void recoveryAvailable(const QString &filePath);

private slots:
    void performAutoSave();

private:
    DocumentModel *m_model;
    QTimer *m_autoSaveTimer;
    bool m_enabled;
    int m_interval;

    bool writeAutoSaveFile(const QString &filePath, const QString &content);
};

#endif // AUTOSAVECONTROLLER_H
