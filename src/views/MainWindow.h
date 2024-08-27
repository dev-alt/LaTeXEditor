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
#include "LatexToolbar.h"
#include "../controllers/LatexToolbarController.h"

class DocumentModel;

class FileController;

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow();
    QPlainTextEdit* getEditor() const;

public slots:
    void updateTheme(const Theme &newTheme);

signals:
    void themeChangeRequested(const QString &themeName);

private slots:
    void changeTheme();
    void removeRTLOverride();
    void debugTextDirection();
    void testTheme();

private:
    void createActions();
    void createMenus();

    QPlainTextEdit *m_editor;
    LaTeXHighlighter *m_highlighter;
    DocumentModel *m_documentModel;
    FileController *m_fileController;
    LatexToolbar *m_latexToolbar;
    LatexToolbarController *m_latexToolbarController;

    QMenu *fileMenu;
    QMenu *viewMenu;

    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *exitAct;

    QActionGroup *themeActGroup;
};

#endif // MAINWINDOW_H