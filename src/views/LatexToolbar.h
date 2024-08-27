#ifndef LATEXTOOLBAR_H
#define LATEXTOOLBAR_H


#include <QToolBar>
#include <QAction>
#include <QMenu>

class LatexToolbar : public QToolBar {
Q_OBJECT

public:
    explicit LatexToolbar(QWidget *parent = nullptr);

signals:
    void commandInsertRequested(const QString &command);

private:
    void createTextFormatActions();

    void createMathActions();

    void createStructureActions();

    void createEnvironmentActions();

    QAction *createAction(const QString &text, const QString &command, const QString &tooltip = "");

    QMenu *createMenu(const QString &title);
};

#endif // LATEXTOOLBAR_H