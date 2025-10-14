// ProjectTreeWidget.cpp
#include "ProjectTreeWidget.h"
#include <QHeaderView>
#include <QFileInfo>
#include <QDebug>

ProjectTreeWidget::ProjectTreeWidget(ProjectModel *projectModel, QWidget *parent)
    : QWidget(parent)
    , m_projectModel(projectModel)
{
    setupUI();

    connect(m_projectModel, &ProjectModel::projectChanged, this, &ProjectTreeWidget::onProjectChanged);
    connect(m_projectModel, &ProjectModel::filesScanned, this, &ProjectTreeWidget::updateTree);
}

void ProjectTreeWidget::setupUI() {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    // Title label
    m_titleLabel = new QLabel(tr("Project Files"), this);
    QFont titleFont = m_titleLabel->font();
    titleFont.setBold(true);
    m_titleLabel->setFont(titleFont);
    m_titleLabel->setMargin(5);
    layout->addWidget(m_titleLabel);

    // Tree widget
    m_treeWidget = new QTreeWidget(this);
    m_treeWidget->setHeaderHidden(true);
    m_treeWidget->setAnimated(true);
    m_treeWidget->setIndentation(20);
    m_treeWidget->setExpandsOnDoubleClick(false);

    connect(m_treeWidget, &QTreeWidget::itemClicked, this, &ProjectTreeWidget::onItemClicked);
    connect(m_treeWidget, &QTreeWidget::itemDoubleClicked, this, &ProjectTreeWidget::onItemDoubleClicked);

    layout->addWidget(m_treeWidget);

    setLayout(layout);
}

void ProjectTreeWidget::updateTree() {
    populateTree();
}

void ProjectTreeWidget::onItemClicked(QTreeWidgetItem *item, int column) {
    Q_UNUSED(column);

    if (item) {
        QString filePath = item->data(0, Qt::UserRole).toString();
        if (!filePath.isEmpty()) {
            emit fileSelected(filePath);
        }
    }
}

void ProjectTreeWidget::onItemDoubleClicked(QTreeWidgetItem *item, int column) {
    Q_UNUSED(column);

    if (item) {
        QString filePath = item->data(0, Qt::UserRole).toString();
        if (!filePath.isEmpty()) {
            emit fileDoubleClicked(filePath);
        }
    }
}

void ProjectTreeWidget::onProjectChanged() {
    updateTree();
}

void ProjectTreeWidget::populateTree() {
    m_treeWidget->clear();

    if (!m_projectModel->hasProject()) {
        // No project loaded
        QTreeWidgetItem *item = new QTreeWidgetItem(m_treeWidget);
        item->setText(0, tr("No project loaded"));
        item->setFlags(Qt::NoItemFlags);
        return;
    }

    // Get project files
    QList<ProjectFile> projectFiles = m_projectModel->getProjectFiles();

    if (projectFiles.isEmpty()) {
        QTreeWidgetItem *item = new QTreeWidgetItem(m_treeWidget);
        item->setText(0, tr("No files in project"));
        item->setFlags(Qt::NoItemFlags);
        return;
    }

    // Find the main file
    QString mainFile = m_projectModel->getMainFile();

    // Create a map to track items by file path
    QMap<QString, QTreeWidgetItem*> itemMap;

    // First pass: Create tree items for all files
    for (const ProjectFile &file : projectFiles) {
        QFileInfo fileInfo(file.filePath);
        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0, file.displayName);
        item->setData(0, Qt::UserRole, file.filePath);
        item->setToolTip(0, file.filePath);

        // Mark main file with special icon/formatting
        if (file.isMainFile) {
            QFont font = item->font(0);
            font.setBold(true);
            item->setFont(0, font);
            item->setText(0, file.displayName + " [Main]");
        }

        itemMap[file.filePath] = item;
    }

    // Second pass: Build hierarchy based on includes
    for (const ProjectFile &file : projectFiles) {
        QTreeWidgetItem *item = itemMap[file.filePath];

        if (file.isMainFile) {
            // Main file goes to root
            m_treeWidget->addTopLevelItem(item);
        } else {
            // Try to find parent (file that includes this one)
            bool foundParent = false;

            for (const ProjectFile &potentialParent : projectFiles) {
                // Check if this file is included by the potential parent
                for (const QString &includedFile : potentialParent.includedFiles) {
                    QString resolvedPath = m_projectModel->resolveIncludePath(
                        potentialParent.filePath, includedFile);

                    if (resolvedPath == file.filePath) {
                        // Found parent
                        QTreeWidgetItem *parentItem = itemMap[potentialParent.filePath];
                        if (parentItem) {
                            parentItem->addChild(item);
                            foundParent = true;
                            break;
                        }
                    }
                }
                if (foundParent) break;
            }

            // If no parent found, add to root (orphaned file)
            if (!foundParent) {
                m_treeWidget->addTopLevelItem(item);
            }
        }
    }

    // Expand all items by default
    m_treeWidget->expandAll();

    // Update title with file count
    m_titleLabel->setText(tr("Project Files (%1)").arg(projectFiles.size()));
}

QTreeWidgetItem* ProjectTreeWidget::findOrCreateFileItem(const QString &filePath, QTreeWidgetItem *parent) {
    QFileInfo fileInfo(filePath);
    QString fileName = fileInfo.fileName();

    // Search for existing item
    QTreeWidgetItemIterator it(m_treeWidget);
    while (*it) {
        if ((*it)->data(0, Qt::UserRole).toString() == filePath) {
            return *it;
        }
        ++it;
    }

    // Create new item
    QTreeWidgetItem *item;
    if (parent) {
        item = new QTreeWidgetItem(parent);
    } else {
        item = new QTreeWidgetItem(m_treeWidget);
    }

    item->setText(0, fileName);
    item->setData(0, Qt::UserRole, filePath);
    item->setToolTip(0, filePath);

    return item;
}
