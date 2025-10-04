// FileController.h
#ifndef FILECONTROLLER_H
#define FILECONTROLLER_H

#include <QObject>
#include <QString>

class DocumentModel;

class MainWindow;

class FileController : public QObject {
Q_OBJECT

public:
    explicit FileController(DocumentModel *model, MainWindow *view, QObject *parent = nullptr);

public slots:

    void newFile();

    void openFile();

    void saveFile();

    void saveFileAs();

    void loadFile(const QString &fileName);

private:
    DocumentModel *m_model;
    MainWindow *m_view;
    QString m_currentFile;

    bool maybeSave();

    bool saveFile(const QString &fileName);

    void setCurrentFile(const QString &fileName);

    void updateEditor();
};

#endif // FILECONTROLLER_H
