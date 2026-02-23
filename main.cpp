#include "mainwindow.h"
#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Nepal Election System");
    app.setOrganizationName("Kathmandu University");

    MainWindow w;
    w.show();

    return app.exec();
}
