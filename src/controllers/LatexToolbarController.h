#ifndef LATEXTOOLBARCONTROLLER_H
#define LATEXTOOLBARCONTROLLER_H

#include <QObject>
#include "../views/LatexToolbar.h"
#include "../models/DocumentModel.h"

class MainWindow;

class LatexToolbarController : public QObject {
Q_OBJECT

public:
    LatexToolbarController(LatexToolbar *view, DocumentModel *model, MainWindow *mainWindow);

private slots:

    void onCommandInsertRequested(const QString &command);

private:
    LatexToolbar *m_view;
    DocumentModel *m_model;
    MainWindow *m_mainWindow;
};

#endif // LATEXTOOLBARCONTROLLER_H
