#include "MainWindow.h"
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    m_editor = new QPlainTextEdit(this);
    setCentralWidget(m_editor);
    resize(800, 600);
    setWindowTitle("LaTeX Editor");
}

QPlainTextEdit* MainWindow::getEditor() const {
    return m_editor;
}