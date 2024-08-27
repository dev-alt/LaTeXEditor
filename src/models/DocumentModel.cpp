#include "DocumentModel.h"

DocumentModel::DocumentModel(QObject *parent) : QObject(parent), m_modified(false) {}

QString DocumentModel::getContent() const {
    return m_content;
}

void DocumentModel::setContent(const QString &content) {
    if (m_content != content) {
        m_content = content;
        m_modified = true;
        emit contentChanged();
    }
}

void DocumentModel::clear() {
    setContent("");
    m_modified = false;
}

bool DocumentModel::isModified() const {
    return m_modified;
}

void DocumentModel::setModified(bool modified) {
    m_modified = modified;
}