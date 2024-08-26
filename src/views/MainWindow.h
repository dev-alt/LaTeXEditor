#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QActionGroup>
#include "../utils/LaTeXHighlighter.h"
#include "../models/Theme.h"

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    QPlainTextEdit* getEditor() const;
    void updateTheme(const Theme &newTheme);

signals:
    void themeChangeRequested(const QString &themeName);

private slots:
    void newFile();
    void openFile();
    void saveFile();
    void changeTheme();

private:
    QPlainTextEdit *m_editor;
    LaTeXHighlighter *m_highlighter;

    void createActions();
    void createMenus();

    QMenu *fileMenu;
    QMenu *viewMenu;
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *exitAct;
    QActionGroup *themeActGroup;
};

#endif // MAINWINDOW_H