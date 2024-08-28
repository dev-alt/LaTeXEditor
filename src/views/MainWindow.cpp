#include "MainWindow.h"
#include <QVBoxLayout>
#include <QFileDialog>
#include <QTextStream>
#include <QIcon>
#include <QActionGroup>
#include <QStatusBar>
#include "../controllers/FileController.h"
#include "../models/DocumentModel.h"
#include "../controllers/LatexToolbarController.h"
#include "../utils/LaTeXHighlighter.h"
#include "../utils/ThemeManager.h"
#include "../controllers/PreviewController.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), m_highlighter(nullptr) {
    qDebug() << "MainWindow constructor started";

    // Create editor
    m_editor = new QPlainTextEdit(this);
    qDebug() << "PlainTextEdit created";

    m_editor->document()->setDefaultTextOption(QTextOption(Qt::AlignLeft | Qt::AlignTop));
    m_editor->setLayoutDirection(Qt::LeftToRight);
    QFont font("Arial", 12);
    m_editor->setFont(font);

    // Initialize DocumentModel
    m_documentModel = new DocumentModel(this);

    // Connect editor's textChanged signal to update the DocumentModel
    connect(m_editor, &QPlainTextEdit::textChanged, this, &MainWindow::updateDocumentModelFromEditor);

    // Initialize PreviewWindow
    m_previewWindow = new PreviewWindow(this);

    // Create a horizontal layout to hold the editor and preview
    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->addWidget(m_editor);
    mainLayout->addWidget(m_previewWindow);
    setCentralWidget(centralWidget);

    // Initialize PreviewController
    m_previewController = new PreviewController(m_documentModel, m_previewWindow, this);

    // Initialize highlighter
    m_highlighter = new LaTeXHighlighter(m_editor->document());

    // Initialize FileController
    m_fileController = new FileController(m_documentModel, this, this);
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
        ThemeManager& themeManager = ThemeManager::getInstance();
        const Theme& initialTheme = themeManager.getCurrentTheme();
        qDebug() << "Initial theme obtained:" << initialTheme.name;
        updateTheme(initialTheme);

        // Connect theme change signal
        connect(&themeManager, &ThemeManager::themeChanged, this, &MainWindow::updateTheme);
    } catch (const std::exception& e) {
        qDebug() << "Exception caught while applying theme:" << e.what();
    } catch (...) {
        qDebug() << "Unknown exception caught while applying theme";
    }

    qDebug() << "MainWindow constructor completed";
}

MainWindow::~MainWindow() {
    delete m_fileController;
    delete m_documentModel;
}


QPlainTextEdit* MainWindow::getEditor() const {
    return m_editor;
}

void MainWindow::createActions() {
    qDebug() << "createActions started";

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

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, &QAction::triggered, this, &QWidget::close);

    rebuildPreviewAct = new QAction(tr("&Rebuild Preview"), this);
    rebuildPreviewAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
    rebuildPreviewAct->setStatusTip(tr("Rebuild the LaTeX preview"));
    connect(rebuildPreviewAct, &QAction::triggered, this, &MainWindow::rebuildPreview);

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
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    viewMenu = menuBar()->addMenu(tr("&View"));
    for (QAction *action : themeActGroup->actions()) {
        viewMenu->addAction(action);
    }
    viewMenu->addSeparator();
    viewMenu->addAction(rebuildPreviewAct);
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
