#ifndef EDITORCONTROLLER_H
#define EDITORCONTROLLER_H

#include <QObject>
#include "models/DocumentModel.h"
#include "views/MainWindow.h"

class EditorController : public QObject {
Q_OBJECT

public:
    explicit EditorController(DocumentModel *model, MainWindow *view, QObject *parent = nullptr);

private slots:
    void onEditorTextChanged();
    void onModelContentChanged();

private:
    DocumentModel *m_model;
    MainWindow *m_view;
};

#endif // EDITORCONTROLLER_H
