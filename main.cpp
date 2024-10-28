#include "mainwindow.h"

#include <QApplication>

// The main function is the entry point of the program
int main(int argc, char *argv[])
{
    // Initializes the QApplication, which handles app-wide resources and settings
    QApplication a(argc, argv);

    // Creates an instance of the MainWindow class, initializing the main user interface and displys the main window on the screen
    MainWindow w;
    w.show();

    return a.exec();
}
