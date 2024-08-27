#include "LatexToolbar.h"

LatexToolbar::LatexToolbar(QWidget *parent) : QToolBar(parent) {
    createActions();
}

void LatexToolbar::createActions() {
    QAction *boldAction = addAction("Bold");
    boldAction->setData("\\textbf{}");
    connect(boldAction, &QAction::triggered, [this, boldAction]() {
        emit commandInsertRequested(boldAction->data().toString());
    });

    QAction *italicAction = addAction("Italic");
    italicAction->setData("\\textit{}");
    connect(italicAction, &QAction::triggered, [this, italicAction]() {
        emit commandInsertRequested(italicAction->data().toString());
    });
}