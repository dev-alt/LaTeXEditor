#include "MainWindow.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    m_editor = new QPlainTextEdit(this);
    setCentralWidget(m_editor);

    // Set light mode for the editor
    QPalette editorPalette = m_editor->palette();
    editorPalette.setColor(QPalette::Base, Qt::white);
    editorPalette.setColor(QPalette::Text, Qt::black);
    m_editor->setPalette(editorPalette);

    m_highlighter = new LaTeXHighlighter(m_editor->document());

    createActions();
    createMenus();

    resize(800, 600);
    setWindowTitle("LaTeX Editor");

    // Set the window icon
    QIcon icon(":/icons/latex_editor_icon.png");
    setWindowIcon(icon);
}
QPlainTextEdit* MainWindow::getEditor() const {
    return m_editor;
}

void MainWindow::createActions() {
    newAct = new QAction(tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);

    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, this, &MainWindow::openFile);

    saveAct = new QAction(tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::saveFile);

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, &QAction::triggered, this, &QWidget::close);
}

void MainWindow::createMenus() {
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);
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