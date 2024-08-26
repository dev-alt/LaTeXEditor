#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPlainTextEdit>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    QPlainTextEdit* getEditor() const;

private:
    QPlainTextEdit *m_editor;
};

#endif // MAINWINDOW_H