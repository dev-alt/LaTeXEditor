#ifndef FINDREPLACEDIALOG_H
#define FINDREPLACEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QPlainTextEdit>

class FindReplaceDialog : public QDialog {
Q_OBJECT

public:
    explicit FindReplaceDialog(QPlainTextEdit *editor, QWidget *parent = nullptr);

private slots:
    void findNext();
    void findPrevious();
    void replace();
    void replaceAll();

private:
    QLineEdit *findLineEdit;
    QLineEdit *replaceLineEdit;
    QPushButton *findNextButton;
    QPushButton *findPreviousButton;
    QPushButton *replaceButton;
    QPushButton *replaceAllButton;
    QCheckBox *caseSensitiveCheckBox;
    QCheckBox *wholeWordsCheckBox;

    QPlainTextEdit *m_editor;

    bool find(bool forward = true);
};

#endif // FINDREPLACEDIALOG_H
