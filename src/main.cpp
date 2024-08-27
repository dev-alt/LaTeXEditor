#include <QApplication>
#include "models/DocumentModel.h"
#include "views/MainWindow.h"
#include "controllers/EditorController.h"
#include <QMessageBox>
int main(int argc, char *argv[]) {
    try {
        QApplication app(argc, argv);
        qDebug() << "Application created";

        MainWindow mainWindow;
        qDebug() << "MainWindow instance created";

        mainWindow.show();
        qDebug() << "MainWindow shown";

        return app.exec();
    } catch (const std::exception& e) {
        qDebug() << "Exception caught:" << e.what();
        QMessageBox::critical(nullptr, "Error", QString("An error occurred: %1").arg(e.what()));
        return 1;
    } catch (...) {
        qDebug() << "Unknown exception caught";
        QMessageBox::critical(nullptr, "Error", "An unknown error occurred");
        return 1;
    }
}
