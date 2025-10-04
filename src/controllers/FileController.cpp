#include "FileController.h"
#include "../models/DocumentModel.h"
#include "../views/MainWindow.h"
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QStatusBar>

FileController::FileController(DocumentModel *model, MainWindow *view, QObject *parent)
        : QObject(parent), m_model(model), m_view(view), m_currentFile("") {
    // Connect the model's contentChanged signal to update the editor
    connect(m_model, &DocumentModel::contentChanged, this, &FileController::updateEditor);
}

void FileController::newFile() {
    if (maybeSave()) {
        m_model->clear();
        setCurrentFile("");
        updateEditor();
    }
}

void FileController::openFile() {
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(m_view);
        if (!fileName.isEmpty()) {
            loadFile(fileName);
        }
    }
}

void FileController::saveFile() {
    if (m_currentFile.isEmpty()) {
        saveFileAs();
    } else {
        saveFile(m_currentFile);
    }
}

void FileController::saveFileAs() {
    QString fileName = QFileDialog::getSaveFileName(m_view, tr("Save As"), QString(),
                                                    tr("LaTeX Files (*.tex);;All Files (*)"));
    if (!fileName.isEmpty()) {
        saveFile(fileName);
    }
}

void FileController::loadFile(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(m_view, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                                     .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return;
    }

    QTextStream in(&file);
    m_model->setContent(in.readAll());
    setCurrentFile(fileName);
    m_view->addToRecentFiles(fileName);
    m_view->statusBar()->showMessage(tr("File loaded"), 2000);
}

bool FileController::saveFile(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(m_view, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                                     .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return false;
    }

    QTextStream out(&file);
    out << m_model->getContent();
    setCurrentFile(fileName);
    m_view->addToRecentFiles(fileName);
    m_view->statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

void FileController::setCurrentFile(const QString &fileName) {
    m_currentFile = fileName;
    m_model->setModified(false);
    m_view->setWindowModified(false);

    QString shownName = m_currentFile;
    if (m_currentFile.isEmpty())
        shownName = "untitled.txt";
    m_view->setWindowFilePath(shownName);
}

void FileController::updateEditor() {
    if (auto *editor = m_view->getEditor()) {
        editor->setPlainText(m_model->getContent());
    }
}

bool FileController::maybeSave() {
    if (!m_model->isModified())
        return true;
    const QMessageBox::StandardButton ret
            = QMessageBox::warning(m_view, tr("Application"),
                                   tr("The document has been modified.\n"
                                      "Do you want to save your changes?"),
                                   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    switch (ret) {
        case QMessageBox::Save:
            return saveFile(m_currentFile);
        case QMessageBox::Cancel:
            return false;
        default:
            break;
    }
    return true;
}