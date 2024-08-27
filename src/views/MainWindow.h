#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QActionGroup>
#include "../utils/LaTeXHighlighter.h"
#include "../utils/ThemeManager.h"

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    QPlainTextEdit* getEditor() const;

public slots:
    void updateTheme(const Theme &newTheme);

signals:
    void themeChangeRequested(const QString &themeName);

private slots:
    void newFile();
    void openFile();
    void saveFile();
    void changeTheme();
    void removeRTLOverride();
    void debugTextDirection();
    void testTheme();
private:
    void createActions();
    void createMenus();

    QPlainTextEdit *m_editor;
    LaTeXHighlighter *m_highlighter;

    QMenu *fileMenu;
    QMenu *viewMenu;

    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *exitAct;

    QActionGroup *themeActGroup;
};

#endif // MAINWINDOW_H