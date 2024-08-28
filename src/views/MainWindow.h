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
#include "PreviewWindow.h"
#include "../controllers/PreviewController.h"

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

    void updateDocumentModelFromEditor();
    void rebuildPreview();

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
    PreviewWindow *m_previewWindow;
    PreviewController *m_previewController;

    QMenu *fileMenu;
    QMenu *viewMenu;

    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *exitAct;
    QAction *rebuildPreviewAct;

    QActionGroup *themeActGroup;
};

#endif // MAINWINDOW_H