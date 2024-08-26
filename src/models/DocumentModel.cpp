#include "DocumentModel.h"

DocumentModel::DocumentModel(QObject *parent) : QObject(parent) {}

QString DocumentModel::getContent() const {
    return m_content;
}

void DocumentModel::setContent(const QString &content) {
    if (m_content != content) {
        m_content = content;
        emit contentChanged();
    }
}