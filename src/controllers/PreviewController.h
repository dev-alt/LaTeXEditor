#ifndef PREVIEWCONTROLLER_H
#define PREVIEWCONTROLLER_H

#include <QObject>
#include <QTimer>
#include "../models/DocumentModel.h"
#include "../views/PreviewWindow.h"

class PreviewController : public QObject {
Q_OBJECT

public:
    explicit PreviewController(DocumentModel *model, PreviewWindow *view, QObject *parent = nullptr);

public slots:

    void updatePreview(const QString &content = QString());
    void schedulePreviewUpdate();

private:
    QString generatePreviewContent(const QString &latexContent);

    DocumentModel *m_model;
    PreviewWindow *m_view;
    QTimer *m_updateTimer;
};

#endif // PREVIEWCONTROLLER_H
