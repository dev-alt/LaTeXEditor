#ifndef PREVIEWCONTROLLER_H
#define PREVIEWCONTROLLER_H

#include <QObject>
#include "../models/DocumentModel.h"
#include "../views/PreviewWindow.h"

class PreviewController : public QObject {
Q_OBJECT

public:
    explicit PreviewController(DocumentModel *model, PreviewWindow *view, QObject *parent = nullptr);

public slots:
    void updatePreview();

private:
    QString generatePreviewContent(const QString &latexContent);

    QString m_lastContent;
    DocumentModel *m_model;
    PreviewWindow *m_view;
};

#endif // PREVIEWCONTROLLER_H
