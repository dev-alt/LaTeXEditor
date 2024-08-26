#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include "LaTeXHighlighter.h"

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    QPlainTextEdit* getEditor() const;

private slots:
    void newFile();
    void openFile();
    void saveFile();

private:
    QPlainTextEdit *m_editor;
    LaTeXHighlighter *m_highlighter;

    void createActions();
    void createMenus();

    QMenu *fileMenu;
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *exitAct;
};

#endif // MAINWINDOW_H
