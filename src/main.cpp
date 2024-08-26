#include <QApplication>
#include "models/DocumentModel.h"
#include "views/MainWindow.h"
#include "controllers/EditorController.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    DocumentModel model;
    MainWindow view;
    EditorController controller(&model, &view);

    view.show();

    return app.exec();
}