#ifndef LATEXTOOLBAR_H
#define LATEXTOOLBAR_H

#include <QToolBar>
#include <QAction>

class LatexToolbar : public QToolBar {
Q_OBJECT

public:
    explicit LatexToolbar(QWidget *parent = nullptr);

signals:

    void commandInsertRequested(const QString &command);

private:
    void createActions();
};

#endif // LATEXTOOLBAR_H