#include <QApplication>
#include <QMainWindow>
#include "ui_mainwindow.h"  // Include the generated UI header

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QMainWindow mainWindow;        // Create the main window
    Ui::MainWindow ui;             // Create an instance of your UI class
    ui.setupUi(&mainWindow);       // Setup the UI on the main window

    mainWindow.show();             // Show the window

    return app.exec();             // Run the application loop
}