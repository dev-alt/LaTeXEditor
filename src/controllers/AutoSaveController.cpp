// AutoSaveController.cpp
#include "AutoSaveController.h"
#include "../models/DocumentModel.h"
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QDebug>

AutoSaveController::AutoSaveController(DocumentModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
    , m_autoSaveTimer(new QTimer(this))
    , m_enabled(true)
    , m_interval(60000)  // Default: 1 minute
{
    m_autoSaveTimer->setInterval(m_interval);
    connect(m_autoSaveTimer, &QTimer::timeout, this, &AutoSaveController::performAutoSave);

    if (m_enabled) {
        m_autoSaveTimer->start();
    }

    // Check for recoverable auto-save on startup
    if (hasRecoverableAutoSave()) {
        emit recoveryAvailable(m_model->getAutoSaveFilePath());
    }
}

AutoSaveController::~AutoSaveController() {
    // Clean up auto-save file when application closes normally
    if (m_model && !m_model->isModified()) {
        deleteAutoSaveFile();
    }
}

void AutoSaveController::setEnabled(bool enabled) {
    m_enabled = enabled;
    if (m_enabled) {
        m_autoSaveTimer->start();
    } else {
        m_autoSaveTimer->stop();
    }
}

bool AutoSaveController::isEnabled() const {
    return m_enabled;
}

void AutoSaveController::setInterval(int milliseconds) {
    m_interval = milliseconds;
    m_autoSaveTimer->setInterval(m_interval);
}

int AutoSaveController::getInterval() const {
    return m_interval;
}

void AutoSaveController::saveNow() {
    performAutoSave();
}

bool AutoSaveController::hasRecoverableAutoSave() const {
    if (!m_model) {
        return false;
    }

    QString autoSavePath = m_model->getAutoSaveFilePath();
    QFileInfo autoSaveInfo(autoSavePath);

    if (!autoSaveInfo.exists()) {
        return false;
    }

    // Check if auto-save file is newer than the original file
    QString currentPath = m_model->getCurrentFilePath();
    if (currentPath.isEmpty()) {
        // Untitled document - auto-save is available
        return true;
    }

    QFileInfo currentInfo(currentPath);
    if (!currentInfo.exists()) {
        // Original file doesn't exist, auto-save is available
        return true;
    }

    // Compare timestamps
    return autoSaveInfo.lastModified() > currentInfo.lastModified();
}

QString AutoSaveController::recoverAutoSave() {
    if (!m_model) {
        return QString();
    }

    QString autoSavePath = m_model->getAutoSaveFilePath();
    QFile file(autoSavePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open auto-save file for recovery:" << autoSavePath;
        return QString();
    }

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    qDebug() << "Recovered content from auto-save file:" << autoSavePath;
    return content;
}

void AutoSaveController::deleteAutoSaveFile() {
    if (!m_model) {
        return;
    }

    QString autoSavePath = m_model->getAutoSaveFilePath();
    QFile file(autoSavePath);

    if (file.exists()) {
        if (file.remove()) {
            qDebug() << "Auto-save file deleted:" << autoSavePath;
        } else {
            qWarning() << "Failed to delete auto-save file:" << autoSavePath;
        }
    }
}

void AutoSaveController::performAutoSave() {
    if (!m_model || !m_enabled) {
        return;
    }

    // Only auto-save if the document has content and is modified
    QString content = m_model->getContent();
    if (content.isEmpty() || !m_model->isModified()) {
        return;
    }

    QString autoSavePath = m_model->getAutoSaveFilePath();

    if (writeAutoSaveFile(autoSavePath, content)) {
        qDebug() << "Auto-saved to:" << autoSavePath;
        emit autoSaved();
    } else {
        QString error = QString("Failed to write auto-save file: %1").arg(autoSavePath);
        qWarning() << error;
        emit autoSaveFailed(error);
    }
}

bool AutoSaveController::writeAutoSaveFile(const QString &filePath, const QString &content) {
    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&file);
    out << content;
    file.close();

    return true;
}
