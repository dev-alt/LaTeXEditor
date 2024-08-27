#ifndef EDITORCONTROLLER_H
#define EDITORCONTROLLER_H

#include <QObject>
#include "../models/DocumentModel.h"
#include "../views/MainWindow.h"
#include "../utils/ThemeManager.h"

class EditorController : public QObject {
Q_OBJECT

public:
    explicit EditorController(DocumentModel *model, MainWindow *view, QObject *parent = nullptr);

private slots:
    void onEditorTextChanged();
    void onModelContentChanged();
    void onThemeChangeRequested(const QString &themeName);

private:
    DocumentModel *m_model;
    MainWindow *m_view;
    ThemeManager &m_themeManager;
};

#endif // EDITORCONTROLLER_H