#include "MainWindow.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QIcon>
#include <QActionGroup>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), m_highlighter(nullptr) {
    qDebug() << "MainWindow constructor started";

    m_editor = new QPlainTextEdit(this);
    qDebug() << "PlainTextEdit created";

    m_editor->document()->setDefaultTextOption(QTextOption(Qt::AlignLeft | Qt::AlignTop));
    m_editor->setLayoutDirection(Qt::LeftToRight);

    QFont font("Arial", 12);
    m_editor->setFont(font);
    qDebug() << "Editor font set";

    setCentralWidget(m_editor);
    qDebug() << "Central widget set";

    // Initialize highlighter
    m_highlighter = new LaTeXHighlighter(m_editor->document());
    qDebug() << "Highlighter created";

    qDebug() << "About to call createActions";
    createActions();
    qDebug() << "Actions created";

    qDebug() << "About to call createMenus";
    createMenus();
    qDebug() << "Menus created";

    resize(800, 600);
    setWindowTitle("LaTeX Editor");
    qDebug() << "Window properties set";

    qDebug() << "About to apply initial theme";
    try {
        ThemeManager& themeManager = ThemeManager::getInstance();
        qDebug() << "ThemeManager instance obtained";
        const Theme& initialTheme = themeManager.getCurrentTheme();
        qDebug() << "Initial theme obtained:" << initialTheme.name;
        updateTheme(initialTheme);
        qDebug() << "updateTheme called";

        // Connect theme change signal
        connect(&themeManager, &ThemeManager::themeChanged, this, &MainWindow::updateTheme);
        qDebug() << "Theme change signal connected";
    } catch (const std::exception& e) {
        qDebug() << "Exception caught while applying theme:" << e.what();
    } catch (...) {
        qDebug() << "Unknown exception caught while applying theme";
    }

    qDebug() << "MainWindow constructor completed";
}


QPlainTextEdit* MainWindow::getEditor() const {
    return m_editor;
}

void MainWindow::createActions() {
    qDebug() << "createActions started";

    newAct = new QAction(tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
    qDebug() << "New action created";

    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, this, &MainWindow::openFile);
    qDebug() << "Open action created";

    saveAct = new QAction(tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::saveFile);
    qDebug() << "Save action created";

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, &QAction::triggered, this, &QWidget::close);
    qDebug() << "Exit action created";

    qDebug() << "About to create theme actions";
    themeActGroup = new QActionGroup(this);
    try {
        for (const QString &themeName : ThemeManager::getInstance().getThemeNames()) {
            QAction *themeAct = new QAction(themeName, this);
            themeAct->setCheckable(true);
            themeActGroup->addAction(themeAct);
            connect(themeAct, &QAction::triggered, this, &MainWindow::changeTheme);
            qDebug() << "Theme action created for:" << themeName;
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
    text.remove(QChar(0x202E)); // Remove RTL override character
    m_editor->setPlainText(text);
}

void MainWindow::debugTextDirection() {
    qDebug() << "Text direction:" << m_editor->document()->defaultTextOption().textDirection();
    qDebug() << "Layout direction:" << m_editor->layoutDirection();
    qDebug() << "First character Unicode:" << (m_editor->toPlainText().isEmpty() ? "Empty" : QString::number(m_editor->toPlainText()[0].unicode()));
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
}

void MainWindow::newFile() {
    if (!m_editor->document()->isEmpty()) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("Application"),
                                   tr("The document has been modified.\n"
                                      "Do you want to save your changes?"),
                                   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            saveFile();
        else if (ret == QMessageBox::Cancel)
            return;
    }

    m_editor->clear();
    setWindowTitle("New File - LaTeX Editor");
}

void MainWindow::openFile() {
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(this, tr("Error"), tr("Could not open file"));
            return;
        }
        QTextStream in(&file);
        m_editor->setPlainText(in.readAll());
        file.close();
        setWindowTitle(fileName + " - LaTeX Editor");
    }
}

void MainWindow::saveFile() {
    QString fileName = QFileDialog::getSaveFileName(this);
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::critical(this, tr("Error"), tr("Could not save file"));
            return;
        }
        QTextStream out(&file);
        out << m_editor->toPlainText();
        file.close();
        setWindowTitle(fileName + " - LaTeX Editor");
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
    qDebug() << "updateTheme called with theme:" << newTheme.name;
    qDebug() << "Window color:" << newTheme.windowColor.name();
    qDebug() << "Text color:" << newTheme.textColor.name();
    qDebug() << "Base color:" << newTheme.baseColor.name();
    qDebug() << "Highlight color:" << newTheme.highlightColor.name();

    if (m_highlighter) {
        m_highlighter->updateTheme(newTheme);
        qDebug() << "Highlighter updated";
    } else {
        qDebug() << "Highlighter is null, skipping update";
    }
    // Update the checked state of theme actions
    for (QAction* action : themeActGroup->actions()) {
        action->setChecked(action->text() == newTheme.name);
    }

    qDebug() << "Theme action checked state updated";
    // Update editor colors
    if (m_editor) {
        QPalette editorPalette = m_editor->palette();
        editorPalette.setColor(QPalette::Base, newTheme.baseColor);
        editorPalette.setColor(QPalette::Text, newTheme.textColor);
        m_editor->setPalette(editorPalette);
        qDebug() << "Editor palette updated";
    } else {
        qDebug() << "Editor is null, skipping palette update";
    }

    // Update main window colors
    QPalette windowPalette = palette();
    windowPalette.setColor(QPalette::Window, newTheme.windowColor);
    windowPalette.setColor(QPalette::WindowText, newTheme.textColor);
    setPalette(windowPalette);
    qDebug() << "Window palette updated";

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
        qDebug() << "Menu bar style updated";
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
    qDebug() << "Menu styles updated";

    // Update status bar if you have one
    if (statusBar()) {
        statusBar()->setPalette(windowPalette);
        qDebug() << "Status bar updated";
    } else {
        qDebug() << "Status bar is null, skipping update";
    }

    // Force an update of the UI
    update();
    qDebug() << "UI update forced";
}