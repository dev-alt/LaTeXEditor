#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QActionGroup>
#include "../utils/LaTeXHighlighter.h"
#include "../utils/ThemeManager.h"
#include "../utils/CodeEditor.h"
#include "../utils/LaTeXErrorChecker.h"
#include "LatexToolbar.h"
#include "../controllers/LatexToolbarController.h"
#include "PreviewWindow.h"
#include "ProjectTreeWidget.h"
#include "../controllers/PreviewController.h"
#include "../controllers/AutoSaveController.h"
#include "../models/ProjectModel.h"
#include <QSettings>
#include <QTimer>
#include <QSplitter>

class DocumentModel;
class FileController;

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    CodeEditor* getEditor() const;
    void addToRecentFiles(const QString &fileName);

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
    void openRecentFile();
    void showFindReplaceDialog();
    void exportToPDF();
    void toggleSpellCheck(bool enabled);
    void newFromTemplate();
    void checkForErrors();
    void showErrorPanel();
    void onProjectFileSelected(const QString &filePath);
    void onProjectFileDoubleClicked(const QString &filePath);
    void toggleProjectTree();
    void setAsMainFile();

private:
    void createActions();
    void createMenus();
    void updateRecentFileActions();
    QString getTemplate(const QString &templateName);

    CodeEditor *m_editor;
    LaTeXHighlighter *m_highlighter;
    DocumentModel *m_documentModel;
    FileController *m_fileController;
    LatexToolbar *m_latexToolbar;
    LatexToolbarController *m_latexToolbarController;
    PreviewWindow *m_previewWindow;
    PreviewController *m_previewController;
    AutoSaveController *m_autoSaveController;
    ProjectModel *m_projectModel;
    ProjectTreeWidget *m_projectTreeWidget;
    LaTeXErrorChecker *m_errorChecker;
    QTimer *m_errorCheckTimer;
    QSplitter *m_mainSplitter;

    QMenu *fileMenu;
    QMenu *viewMenu;
    QMenu *editMenu;
    QMenu *recentFilesMenu;
    QMenu *templatesMenu;

    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *exportPDFAct;
    QAction *exitAct;
    QAction *findReplaceAct;
    QAction *spellCheckAct;
    QAction *showErrorsAct;
    QAction *rebuildPreviewAct;
    QAction *toggleProjectTreeAct;
    QAction *setAsMainFileAct;

    QActionGroup *themeActGroup;

    static const int MaxRecentFiles = 10;
    QAction *recentFileActs[MaxRecentFiles];
    QSettings m_settings;
};

#endif // MAINWINDOW_H