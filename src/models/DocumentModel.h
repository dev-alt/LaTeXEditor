// DocumentModel.h
#ifndef DOCUMENTMODEL_H
#define DOCUMENTMODEL_H

#include <QString>
#include <QObject>

class DocumentModel : public QObject {
Q_OBJECT

public:
    explicit DocumentModel(QObject *parent = nullptr);

    QString getContent() const;
    void setContent(const QString &content);

    void clear();

    bool isModified() const;

    void setModified(bool modified);

signals:
    void contentChanged();

private:
    QString m_content;
    bool m_modified;
};

#endif // DOCUMENTMODEL_H