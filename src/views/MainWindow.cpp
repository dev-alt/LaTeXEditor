#include "MainWindow.h"
#include "FindReplaceDialog.h"
#include <QVBoxLayout>
#include <QFileDialog>
#include <QTextStream>
#include <QIcon>
#include <QActionGroup>
#include <QStatusBar>
#include <QMessageBox>
#include <QPrinter>
#include <QPrintDialog>
#include "../controllers/FileController.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), m_highlighter(nullptr) {
    try {
    qDebug() << "MainWindow constructor started";

    // Create editor
    m_editor = new CodeEditor(this);
    qDebug() << "CodeEditor created";

    m_editor->document()->setDefaultTextOption(QTextOption(Qt::AlignLeft | Qt::AlignTop));
    m_editor->setLayoutDirection(Qt::LeftToRight);
    QFont font("Arial", 12);
    m_editor->setFont(font);

    // Initialize DocumentModel
    m_documentModel = new DocumentModel(this);

    // Connect editor's textChanged signal to update the DocumentModel
    connect(m_editor, &QPlainTextEdit::textChanged, this, &MainWindow::updateDocumentModelFromEditor);

    // Initialize ProjectModel
    m_projectModel = new ProjectModel(this);

    // Initialize ProjectTreeWidget
    m_projectTreeWidget = new ProjectTreeWidget(m_projectModel, this);
    m_projectTreeWidget->setMinimumWidth(200);
    m_projectTreeWidget->setMaximumWidth(400);
    connect(m_projectTreeWidget, &ProjectTreeWidget::fileSelected,
            this, &MainWindow::onProjectFileSelected);
    connect(m_projectTreeWidget, &ProjectTreeWidget::fileDoubleClicked,
            this, &MainWindow::onProjectFileDoubleClicked);

    // Initialize PreviewWindow
    m_previewWindow = new PreviewWindow(this);

    // Create splitters for flexible layout
    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Main horizontal splitter: [Project Tree | Editor | Preview]
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);
    m_mainSplitter->addWidget(m_projectTreeWidget);
    m_mainSplitter->addWidget(m_editor);
    m_mainSplitter->addWidget(m_previewWindow);

    // Set initial sizes: 20% tree, 40% editor, 40% preview
    m_mainSplitter->setStretchFactor(0, 1);
    m_mainSplitter->setStretchFactor(1, 2);
    m_mainSplitter->setStretchFactor(2, 2);

    mainLayout->addWidget(m_mainSplitter);
    setCentralWidget(centralWidget);

    // Initialize PreviewController
    m_previewController = new PreviewController(m_documentModel, m_previewWindow, this);

    // Initialize highlighter
    m_highlighter = new LaTeXHighlighter(m_editor->document());

    // Initialize spell checker
    m_spellChecker = new SpellChecker(this);
    QString affixPath = SpellChecker::getDefaultAffixPath();
    QString dictPath = SpellChecker::getDefaultDictionaryPath();

    if (!affixPath.isEmpty() && !dictPath.isEmpty()) {
        if (m_spellChecker->initialize(affixPath, dictPath)) {
            qDebug() << "Spell checker initialized successfully";
            // Load personal dictionary
            m_spellChecker->loadPersonalDictionary(SpellChecker::getDefaultPersonalDictionaryPath());
        } else {
            qDebug() << "Failed to initialize spell checker";
        }
    } else {
        qDebug() << "Hunspell dictionaries not found. Spell checking disabled.";
        qDebug() << "Install en_US dictionary to enable spell checking";
    }

    // Initialize spell check highlighter (starts disabled)
    m_spellCheckHighlighter = new SpellCheckHighlighter(m_spellChecker, m_editor->document());
    m_spellCheckHighlighter->setEnabled(false);

    // Set spell checker in editor for context menu
    m_editor->setSpellChecker(m_spellChecker);

    // Initialize error checker
    m_errorChecker = new LaTeXErrorChecker(this);
    m_errorCheckTimer = new QTimer(this);
    m_errorCheckTimer->setSingleShot(true);
    m_errorCheckTimer->setInterval(1000); // 1 second delay
    connect(m_errorCheckTimer, &QTimer::timeout, this, &MainWindow::checkForErrors);
    connect(m_editor, &CodeEditor::textChanged, m_errorCheckTimer, qOverload<>(&QTimer::start));

    // Initialize FileController
    m_fileController = new FileController(m_documentModel, this, this);

    // Initialize AutoSaveController
    m_autoSaveController = new AutoSaveController(m_documentModel, this);
    connect(m_autoSaveController, &AutoSaveController::autoSaved, this, [this]() {
        statusBar()->showMessage(tr("Auto-saved"), 2000);
    });
    connect(m_autoSaveController, &AutoSaveController::autoSaveFailed, this, [this](const QString &error) {
        statusBar()->showMessage(tr("Auto-save failed: %1").arg(error), 5000);
    });

    // Check for auto-save recovery
    if (m_autoSaveController->hasRecoverableAutoSave()) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this,
            tr("Recover Auto-saved Document"),
            tr("An auto-saved version of your document was found. Would you like to recover it?"),
            QMessageBox::Yes | QMessageBox::No
        );

        if (reply == QMessageBox::Yes) {
            QString content = m_autoSaveController->recoverAutoSave();
            if (!content.isEmpty()) {
                m_editor->setPlainText(content);
                m_documentModel->setContent(content);
                statusBar()->showMessage(tr("Document recovered from auto-save"), 5000);
            }
        } else {
            m_autoSaveController->deleteAutoSaveFile();
        }
    }

    createActions();
    createMenus();

    // Create rebuild preview action
    rebuildPreviewAct = new QAction(tr("Rebuild Preview"), this);
    rebuildPreviewAct->setStatusTip(tr("Rebuild the LaTeX preview"));
    connect(rebuildPreviewAct, &QAction::triggered, this, &MainWindow::rebuildPreview);

    // Initialize and add LatexToolbar
    m_latexToolbar = new LatexToolbar(this);
    addToolBar(Qt::TopToolBarArea, m_latexToolbar);

    // Initialize LatexToolbarController
    m_latexToolbarController = new LatexToolbarController(m_latexToolbar, m_documentModel, this);

    // Add rebuild preview action to toolbar
    QToolBar *toolBar = addToolBar(tr("Tools"));
    toolBar->addAction(rebuildPreviewAct);

    resize(800, 600);
    setWindowTitle("LaTeX Editor");

        try {
            ThemeManager &themeManager = ThemeManager::getInstance();
            const Theme &initialTheme = themeManager.getCurrentTheme();
            qDebug() << "Initial theme obtained:" << initialTheme.name;
            updateTheme(initialTheme);

            // Connect theme change signal
            connect(&themeManager, &ThemeManager::themeChanged, this, &MainWindow::updateTheme);
        } catch (const std::exception &e) {
            qDebug() << "Exception caught while applying theme:" << e.what();
            QMessageBox::warning(this, "Theme Error", QString("Error applying theme: %1").arg(e.what()));
        } catch (...) {
            qDebug() << "Unknown exception caught while applying theme";
            QMessageBox::warning(this, "Theme Error", "Unknown error occurred while applying theme");
        }

        qDebug() << "MainWindow constructor completed";
    } catch (const std::exception& e) {
        qDebug() << "Exception caught in MainWindow constructor:" << e.what();
        QMessageBox::critical(this, "Initialization Error", QString("Error initializing MainWindow: %1").arg(e.what()));
        throw; // Re-throw the exception to be caught in main()
    } catch (...) {
        qDebug() << "Unknown exception caught in MainWindow constructor";
        QMessageBox::critical(this, "Initialization Error", "Unknown error occurred while initializing MainWindow");
        throw; // Re-throw the exception to be caught in main()
    }
}


MainWindow::~MainWindow() {
    // Save personal dictionary before exit
    if (m_spellChecker && m_spellChecker->isInitialized()) {
        m_spellChecker->savePersonalDictionary(SpellChecker::getDefaultPersonalDictionaryPath());
    }

    // Qt's parent-child ownership handles cleanup automatically
    // All objects created with 'this' as parent are deleted when MainWindow is destroyed
}


CodeEditor* MainWindow::getEditor() const {
    return m_editor;
}

void MainWindow::createActions() {
    qDebug() << "createActions started";

    // Initialize recent file actions
    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], &QAction::triggered, this, &MainWindow::openRecentFile);
    }

    newAct = new QAction(tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, &QAction::triggered, m_fileController, &FileController::newFile);

    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, m_fileController, &FileController::openFile);


    saveAct = new QAction(tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, &QAction::triggered, [this]() { m_fileController->saveFile(); });

    saveAsAct = new QAction(tr("Save &As..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    connect(saveAsAct, &QAction::triggered, m_fileController, &FileController::saveFileAs);

    exportPDFAct = new QAction(tr("Export to &PDF..."), this);
    exportPDFAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
    exportPDFAct->setStatusTip(tr("Export document to PDF"));
    connect(exportPDFAct, &QAction::triggered, this, &MainWindow::exportToPDF);

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, &QAction::triggered, this, &QWidget::close);

    findReplaceAct = new QAction(tr("&Find and Replace..."), this);
    findReplaceAct->setShortcuts(QKeySequence::Find);
    findReplaceAct->setStatusTip(tr("Find and replace text"));
    connect(findReplaceAct, &QAction::triggered, this, &MainWindow::showFindReplaceDialog);

    spellCheckAct = new QAction(tr("Enable &Spell Checking"), this);
    spellCheckAct->setCheckable(true);
    spellCheckAct->setChecked(false);
    spellCheckAct->setStatusTip(tr("Enable or disable spell checking"));
    connect(spellCheckAct, &QAction::toggled, this, &MainWindow::toggleSpellCheck);

    showErrorsAct = new QAction(tr("Show &Errors"), this);
    showErrorsAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_L));
    showErrorsAct->setStatusTip(tr("Show LaTeX syntax errors"));
    connect(showErrorsAct, &QAction::triggered, this, &MainWindow::showErrorPanel);

    rebuildPreviewAct = new QAction(tr("&Rebuild Preview"), this);
    rebuildPreviewAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
    rebuildPreviewAct->setStatusTip(tr("Rebuild the LaTeX preview"));
    connect(rebuildPreviewAct, &QAction::triggered, this, &MainWindow::rebuildPreview);

    toggleProjectTreeAct = new QAction(tr("Toggle &Project Tree"), this);
    toggleProjectTreeAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));
    toggleProjectTreeAct->setStatusTip(tr("Show or hide the project tree"));
    toggleProjectTreeAct->setCheckable(true);
    toggleProjectTreeAct->setChecked(true);
    connect(toggleProjectTreeAct, &QAction::triggered, this, &MainWindow::toggleProjectTree);

    setAsMainFileAct = new QAction(tr("Set as &Main File"), this);
    setAsMainFileAct->setStatusTip(tr("Set the current file as the main project file"));
    connect(setAsMainFileAct, &QAction::triggered, this, &MainWindow::setAsMainFile);

    themeActGroup = new QActionGroup(this);
    try {
        for (const QString &themeName : ThemeManager::getInstance().getThemeNames()) {
            QAction *themeAct = new QAction(themeName, this);
            themeAct->setCheckable(true);
            themeActGroup->addAction(themeAct);
            connect(themeAct, SIGNAL(triggered()), this, SLOT(changeTheme()));
        }
    } catch (const std::exception& e) {
        qDebug() << "Exception caught while creating theme actions:" << e.what();
    } catch (...) {
        qDebug() << "Unknown exception caught while creating theme actions";
    }

    qDebug() << "createActions completed";
}

void MainWindow::removeRTLOverride() {
    QString text = m_editor->toPlainText();
    text.remove(QChar(0x202E));
    m_editor->setPlainText(text);
}

void MainWindow::debugTextDirection() {
    qDebug() << "Text direction:" << m_editor->document()->defaultTextOption().textDirection();
    qDebug() << "Layout direction:" << m_editor->layoutDirection();
    qDebug() << "First character Unicode:" << (m_editor->toPlainText().isEmpty() ? "Empty" : QString::number(m_editor->toPlainText()[0].unicode()));
}

void MainWindow::updateDocumentModelFromEditor() {
    // Store the current cursor position
    QTextCursor cursor = m_editor->textCursor();
    int position = cursor.position();

    // Get the content and update the model
    QString content = m_editor->toPlainText();
    m_documentModel->setContent(content);

    // Restore the cursor position
    cursor.setPosition(position);
    m_editor->setTextCursor(cursor);
}


void MainWindow::createMenus() {
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);

    // Add templates submenu
    templatesMenu = fileMenu->addMenu(tr("New from Template"));
    QAction *articleAct = new QAction(tr("Article"), this);
    connect(articleAct, &QAction::triggered, [this]() {
        m_editor->setPlainText(getTemplate("article"));
        m_documentModel->setContent(m_editor->toPlainText());
    });
    templatesMenu->addAction(articleAct);

    QAction *reportAct = new QAction(tr("Report"), this);
    connect(reportAct, &QAction::triggered, [this]() {
        m_editor->setPlainText(getTemplate("report"));
        m_documentModel->setContent(m_editor->toPlainText());
    });
    templatesMenu->addAction(reportAct);

    QAction *beamerAct = new QAction(tr("Beamer Presentation"), this);
    connect(beamerAct, &QAction::triggered, [this]() {
        m_editor->setPlainText(getTemplate("beamer"));
        m_documentModel->setContent(m_editor->toPlainText());
    });
    templatesMenu->addAction(beamerAct);

    QAction *letterAct = new QAction(tr("Letter"), this);
    connect(letterAct, &QAction::triggered, [this]() {
        m_editor->setPlainText(getTemplate("letter"));
        m_documentModel->setContent(m_editor->toPlainText());
    });
    templatesMenu->addAction(letterAct);

    fileMenu->addAction(openAct);

    // Add recent files submenu
    recentFilesMenu = fileMenu->addMenu(tr("Recent Files"));
    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFilesMenu->addAction(recentFileActs[i]);
    }
    updateRecentFileActions();

    fileMenu->addSeparator();
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exportPDFAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(findReplaceAct);
    editMenu->addSeparator();
    editMenu->addAction(spellCheckAct);

    viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(toggleProjectTreeAct);
    viewMenu->addSeparator();
    for (QAction *action : themeActGroup->actions()) {
        viewMenu->addAction(action);
    }
    viewMenu->addSeparator();
    viewMenu->addAction(rebuildPreviewAct);
    viewMenu->addAction(showErrorsAct);

    // Add Project menu
    QMenu *projectMenu = menuBar()->addMenu(tr("&Project"));
    projectMenu->addAction(setAsMainFileAct);
}

void MainWindow::rebuildPreview() {
    if (m_previewController && m_editor) {
        QString currentContent = m_editor->toPlainText();
        m_previewController->updatePreview(currentContent);
    }
}

void MainWindow::changeTheme() {
    QAction *themeAct = qobject_cast<QAction*>(sender());
    if (themeAct) {
        qDebug() << "Changing theme to:" << themeAct->text();
        ThemeManager::getInstance().applyTheme(themeAct->text());
    }
}

void MainWindow::testTheme() {
    qDebug() << "Test theme button clicked";
    Theme testTheme("Test", Qt::red, Qt::white, Qt::black, Qt::yellow, Qt::blue, Qt::green, Qt::cyan, Qt::magenta, Qt::darkGreen, Qt::gray);
    updateTheme(testTheme);
}

void MainWindow::updateTheme(const Theme &newTheme) {

    if (m_highlighter) {
        m_highlighter->updateTheme(newTheme);
    } else {
        qDebug() << "Highlighter is null, skipping update";
    }

    // Update the checked state of theme actions
    for (QAction* action : themeActGroup->actions()) {
        action->setChecked(action->text() == newTheme.name);
    }

    // Update editor colors
    if (m_editor) {
        QPalette editorPalette = m_editor->palette();
        editorPalette.setColor(QPalette::Base, newTheme.baseColor);
        editorPalette.setColor(QPalette::Text, newTheme.textColor);
        m_editor->setPalette(editorPalette);
    } else {
        qDebug() << "Editor is null, skipping palette update";
    }

    // Update main window colors
    QPalette windowPalette = palette();
    windowPalette.setColor(QPalette::Window, newTheme.windowColor);
    windowPalette.setColor(QPalette::WindowText, newTheme.textColor);
    setPalette(windowPalette);

    // Update toolbar styles
    if (m_latexToolbar) {
        QString toolbarStyle = QString(
                "QToolBar { background-color: %1; color: %2; }"
                "QToolBar QToolButton { background-color: %1; color: %2; border: 1px solid %3; }"
                "QToolBar QToolButton:hover { background-color: %4; }"
        ).arg(newTheme.windowColor.name())
                .arg(newTheme.textColor.name())
                .arg(newTheme.highlightColor.name())
                .arg(newTheme.highlightColor.name());

        m_latexToolbar->setStyleSheet(toolbarStyle);
    } else {
        qDebug() << "LatexToolbar is null, skipping style update";
    }

    // Set a specific style for the menu bar to ensure readability
    QString menuBarStyle = QString(
            "QMenuBar { background-color: %1; color: %2; }"
            "QMenuBar::item { background-color: transparent; }"
            "QMenuBar::item:selected { background-color: %3; color: %4; }"
    ).arg(newTheme.windowColor.name())
            .arg(newTheme.textColor.name())
            .arg(newTheme.highlightColor.name())
            .arg(newTheme.highlightedTextColor.name());

    if (menuBar()) {
        menuBar()->setStyleSheet(menuBarStyle);
    } else {
        qDebug() << "Menu bar is null, skipping style update";
    }

    // Set a style for dropdown menus
    QString menuStyle = QString(
            "QMenu { background-color: %1; color: %2; }"
            "QMenu::item:selected { background-color: %3; color: %4; }"
    ).arg(newTheme.windowColor.name())
            .arg(newTheme.textColor.name())
            .arg(newTheme.highlightColor.name())
            .arg(newTheme.highlightedTextColor.name());

    if (fileMenu) {
        fileMenu->setStyleSheet(menuStyle);
    }
    if (viewMenu) {
        viewMenu->setStyleSheet(menuStyle);
    }

    // Update status bar if you have one
    if (statusBar()) {
        statusBar()->setPalette(windowPalette);
    } else {
        qDebug() << "Status bar is null, skipping update";
    }

    if (m_previewWindow) {
        m_previewWindow->updateTheme(newTheme);
    } else {
        qDebug() << "Preview window is null, skipping theme update";
    }

    // Force an update of the UI
    update();
    qDebug() << "UI update forced";
}

void MainWindow::updateRecentFileActions() {
    QStringList files = m_settings.value("recentFileList").toStringList();

    int numRecentFiles = qMin(files.size(), MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = tr("&%1 %2").arg(i + 1).arg(QFileInfo(files[i]).fileName());
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(files[i]);
        recentFileActs[i]->setVisible(true);
    }

    for (int i = numRecentFiles; i < MaxRecentFiles; ++i) {
        recentFileActs[i]->setVisible(false);
    }

    recentFilesMenu->setEnabled(numRecentFiles > 0);
}

void MainWindow::addToRecentFiles(const QString &fileName) {
    QStringList files = m_settings.value("recentFileList").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MaxRecentFiles) {
        files.removeLast();
    }

    m_settings.setValue("recentFileList", files);
    updateRecentFileActions();
}

void MainWindow::openRecentFile() {
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        QString fileName = action->data().toString();
        if (QFile::exists(fileName)) {
            m_fileController->loadFile(fileName);
        } else {
            QMessageBox::warning(this, tr("File Not Found"),
                               tr("The file %1 no longer exists.").arg(fileName));
            // Remove from recent files list
            QStringList files = m_settings.value("recentFileList").toStringList();
            files.removeAll(fileName);
            m_settings.setValue("recentFileList", files);
            updateRecentFileActions();
        }
    }
}

void MainWindow::showFindReplaceDialog() {
    FindReplaceDialog *dialog = new FindReplaceDialog(m_editor, this);
    dialog->show();
}

void MainWindow::exportToPDF() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export to PDF"), QString(),
                                                    tr("PDF Files (*.pdf)"));
    if (!fileName.isEmpty()) {
        if (!fileName.endsWith(".pdf", Qt::CaseInsensitive)) {
            fileName += ".pdf";
        }

        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(fileName);
        printer.setPageSize(QPageSize(QPageSize::A4));

        m_editor->document()->print(&printer);

        statusBar()->showMessage(tr("PDF exported successfully"), 3000);
    }
}

void MainWindow::toggleSpellCheck(bool enabled) {
    if (m_spellCheckHighlighter) {
        m_spellCheckHighlighter->setEnabled(enabled);

        if (enabled) {
            if (m_spellChecker && m_spellChecker->isInitialized()) {
                statusBar()->showMessage(tr("Spell checking enabled"), 3000);
            } else {
                statusBar()->showMessage(tr("Spell checking unavailable - dictionaries not found"), 5000);
                spellCheckAct->setChecked(false);
            }
        } else {
            statusBar()->showMessage(tr("Spell checking disabled"), 3000);
        }
    }
}

void MainWindow::newFromTemplate() {
    // This is called from template menu items
}

void MainWindow::checkForErrors() {
    QString content = m_editor->toPlainText();
    QVector<LaTeXError> errors = m_errorChecker->checkDocument(content);
    m_editor->setErrors(errors);

    // Update status bar
    if (errors.isEmpty()) {
        statusBar()->showMessage(tr("No errors found"), 2000);
    } else {
        statusBar()->showMessage(tr("%1 error(s) found").arg(errors.size()), 5000);
    }
}

void MainWindow::showErrorPanel() {
    QVector<LaTeXError> errors = m_editor->getErrors();

    if (errors.isEmpty()) {
        QMessageBox::information(this, tr("LaTeX Errors"),
                               tr("No syntax errors found in the document."));
        return;
    }

    QString errorText = tr("Found %1 error(s):\n\n").arg(errors.size());

    for (const LaTeXError &error : errors) {
        QString errorType;
        switch (error.type) {
            case LaTeXError::UnmatchedBrace:
                errorType = "Unmatched Brace";
                break;
            case LaTeXError::UnmatchedBracket:
                errorType = "Unmatched Bracket";
                break;
            case LaTeXError::UnmatchedEnvironment:
                errorType = "Unmatched Environment";
                break;
            case LaTeXError::UnmatchedMathDelimiter:
                errorType = "Unmatched Math Delimiter";
                break;
            case LaTeXError::InvalidCommand:
                errorType = "Invalid Command";
                break;
            case LaTeXError::MissingArgument:
                errorType = "Missing Argument";
                break;
            case LaTeXError::UnknownCommand:
                errorType = "Unknown Command";
                break;
            case LaTeXError::MissingPackage:
                errorType = "Missing Package";
                break;
            case LaTeXError::UsePackageAfterBeginDocument:
                errorType = "Package After \\begin{document}";
                break;
            case LaTeXError::InvalidArgumentCount:
                errorType = "Invalid Argument Count";
                break;
            case LaTeXError::DeprecatedCommand:
                errorType = "Deprecated Command";
                break;
            case LaTeXError::MathModeRequired:
                errorType = "Math Mode Required";
                break;
            case LaTeXError::TextModeRequired:
                errorType = "Text Mode Required";
                break;
        }

        errorText += QString("Line %1, Col %2: [%3] %4\n")
                         .arg(error.line + 1)
                         .arg(error.column + 1)
                         .arg(errorType)
                         .arg(error.message);
    }

    QMessageBox msgBox(this);
    msgBox.setWindowTitle(tr("LaTeX Syntax Errors"));
    msgBox.setText(errorText);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.exec();
}

QString MainWindow::getTemplate(const QString &templateName) {
    if (templateName == "article") {
        return R"(\documentclass[12pt]{article}
\usepackage[utf8]{inputenc}
\usepackage{amsmath}
\usepackage{graphicx}

\title{Your Title Here}
\author{Your Name}
\date{\today}

\begin{document}

\maketitle

\begin{abstract}
Your abstract goes here.
\end{abstract}

\section{Introduction}
Your introduction goes here.

\section{Main Content}
Your main content goes here.

\section{Conclusion}
Your conclusion goes here.

\bibliographystyle{plain}
\bibliography{references}

\end{document})";
    } else if (templateName == "report") {
        return R"(\documentclass[12pt]{report}
\usepackage[utf8]{inputenc}
\usepackage{amsmath}
\usepackage{graphicx}

\title{Your Report Title}
\author{Your Name}
\date{\today}

\begin{document}

\maketitle

\tableofcontents

\chapter{Introduction}
Your introduction goes here.

\chapter{Main Content}
Your main content goes here.

\chapter{Conclusion}
Your conclusion goes here.

\bibliographystyle{plain}
\bibliography{references}

\end{document})";
    } else if (templateName == "beamer") {
        return R"(\documentclass{beamer}
\usetheme{Madrid}

\title{Your Presentation Title}
\author{Your Name}
\date{\today}

\begin{document}

\frame{\titlepage}

\begin{frame}
\frametitle{Outline}
\tableofcontents
\end{frame}

\section{Introduction}
\begin{frame}
\frametitle{Introduction}
Your introduction slide content here.
\end{frame}

\section{Main Content}
\begin{frame}
\frametitle{Main Content}
Your main content here.
\end{frame}

\section{Conclusion}
\begin{frame}
\frametitle{Conclusion}
Your conclusion here.
\end{frame}

\end{document})";
    } else if (templateName == "letter") {
        return R"(\documentclass{letter}
\usepackage[utf8]{inputenc}

\signature{Your Name}
\address{Your Address \\ City, State ZIP}

\begin{document}

\begin{letter}{Recipient Name \\ Recipient Address \\ City, State ZIP}

\opening{Dear Sir or Madam,}

Your letter content goes here.

\closing{Sincerely,}

\end{letter}

\end{document})";
    }
    return "";
}

void MainWindow::onProjectFileSelected(const QString &filePath) {
    qDebug() << "Project file selected:" << filePath;
    // Could implement preview or highlight in the future
}

void MainWindow::onProjectFileDoubleClicked(const QString &filePath) {
    qDebug() << "Project file double-clicked:" << filePath;

    // Load the file into the editor
    if (QFile::exists(filePath)) {
        m_fileController->loadFile(filePath);
    } else {
        QMessageBox::warning(this, tr("File Not Found"),
                           tr("The file %1 does not exist.").arg(filePath));
    }
}

void MainWindow::toggleProjectTree() {
    bool visible = m_projectTreeWidget->isVisible();
    m_projectTreeWidget->setVisible(!visible);
    toggleProjectTreeAct->setChecked(!visible);
}

void MainWindow::setAsMainFile() {
    QString currentFile = m_documentModel->getCurrentFilePath();

    if (currentFile.isEmpty()) {
        QMessageBox::information(this, tr("No File"),
                               tr("Please save the current file first before setting it as the main file."));
        return;
    }

    m_projectModel->setMainFile(currentFile);
    statusBar()->showMessage(tr("Set %1 as main project file").arg(QFileInfo(currentFile).fileName()), 3000);
}
