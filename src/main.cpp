#include <QApplication>
#include <QIcon>
#include <QPalette>
#include "views/MainWindow.h"
#include "models/DocumentModel.h"
#include "controllers/EditorController.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Set the application icon
    QIcon icon(":/icons/latex_editor_icon.png");
    app.setWindowIcon(icon);

    // Force the application to use light mode
    QPalette lightPalette = app.palette();
    lightPalette.setColor(QPalette::Window, QColor(240, 240, 240));
    lightPalette.setColor(QPalette::WindowText, Qt::black);
    lightPalette.setColor(QPalette::Base, Qt::white);
    lightPalette.setColor(QPalette::AlternateBase, QColor(245, 245, 245));
    lightPalette.setColor(QPalette::ToolTipBase, Qt::white);
    lightPalette.setColor(QPalette::ToolTipText, Qt::black);
    lightPalette.setColor(QPalette::Text, Qt::black);
    lightPalette.setColor(QPalette::Button, QColor(240, 240, 240));
    lightPalette.setColor(QPalette::ButtonText, Qt::black);
    lightPalette.setColor(QPalette::BrightText, Qt::red);
    lightPalette.setColor(QPalette::Link, QColor(0, 0, 255));
    lightPalette.setColor(QPalette::Highlight, QColor(76, 163, 224));
    lightPalette.setColor(QPalette::HighlightedText, Qt::white);
    app.setPalette(lightPalette);

    MainWindow view;
    DocumentModel model;
    EditorController controller(&model, &view);

    view.show();

    return app.exec();
}