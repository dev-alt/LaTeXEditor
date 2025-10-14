// ProjectTreeWidget.h
#ifndef PROJECTTREEWIDGET_H
#define PROJECTTREEWIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include "../models/ProjectModel.h"

class ProjectTreeWidget : public QWidget {
Q_OBJECT

public:
    explicit ProjectTreeWidget(ProjectModel *projectModel, QWidget *parent = nullptr);

    void updateTree();

signals:
    void fileSelected(const QString &filePath);
    void fileDoubleClicked(const QString &filePath);

private slots:
    void onItemClicked(QTreeWidgetItem *item, int column);
    void onItemDoubleClicked(QTreeWidgetItem *item, int column);
    void onProjectChanged();

private:
    ProjectModel *m_projectModel;
    QTreeWidget *m_treeWidget;
    QLabel *m_titleLabel;

    void setupUI();
    void populateTree();
    QTreeWidgetItem* findOrCreateFileItem(const QString &filePath, QTreeWidgetItem *parent = nullptr);
};

#endif // PROJECTTREEWIDGET_H
