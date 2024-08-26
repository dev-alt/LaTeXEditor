#include <QApplication>
#include "views/MainWindow.h"
#include "models/DocumentModel.h"
#include "controllers/EditorController.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Set the application icon
    QIcon icon(":/icons/latex_editor_icon.png");
    app.setWindowIcon(icon);

    MainWindow view;
    DocumentModel model;
    EditorController controller(&model, &view);

    view.show();

    return app.exec();
}