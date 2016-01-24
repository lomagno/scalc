#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    qApp->setAttribute(Qt::AA_DontShowIconsInMenus, false);
    MainWindow mainWindow;
    mainWindow.showMaximized();
    return application.exec();
}
