#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>
#include <QString>
#include "exportbodeplot.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// The MainWindow class manages the user interface and interactions for plotting and exporting bode plots
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // Initializes the user interface and connections
    MainWindow(QWidget *parent = nullptr);

    // Cleans up resources
    ~MainWindow();

private slots:
    // Generates and displays the bode plot
    void plotBode();

    // Updates the transfer function display based on user inputs
    void updateTransferFunctionDisplay();

    // Initiates the export process for the bode plots
    void onExportButtonClicked();

private:
    // Converts a comma-separated string into a vector of coefficients
    std::vector<double> parseInput(const QString &input);

    // Points to the user interface elements
    Ui::MainWindow *ui;

    // Handles the export functionality for the bode plots
    ExportBodePlot *exporter;
};

#endif
