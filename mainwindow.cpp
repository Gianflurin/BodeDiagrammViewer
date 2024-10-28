#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "transferfunction.h"
#include "bodeplot.h"
#include "exportbodeplot.h"
#include <QFileDialog>

// Constructor for the MainWindow
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Connects changes in the numerator and denominator inputs to the transfer function display
    connect(ui->numeratorInput, &QLineEdit::textChanged, this, &MainWindow::updateTransferFunctionDisplay);
    connect(ui->denominatorInput, &QLineEdit::textChanged, this, &MainWindow::updateTransferFunctionDisplay);

    // Plots the bode plot when the 'Start' button is clicked
    connect(ui->plotButton, &QPushButton::clicked, this, &MainWindow::plotBode);

    // Initializes the ExportBodePlot class for exporting the magnitude and phase plots
    exporter = new ExportBodePlot(ui->magnitudePlot, ui->phasePlot);

    // Populates the combo box with available export formats
    ui->exportComboBox->addItem("PNG");
    ui->exportComboBox->addItem("JPG");
    ui->exportComboBox->addItem("PDF");
    ui->exportComboBox->addItem("SVG");

    // Connects the 'Exportieren' button to the export function
    connect(ui->exportButton, &QPushButton::clicked, this, &MainWindow::onExportButtonClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Helper function to parse the inputs of the coefficients from the QLineEdit field
// Converts a comma-separated string into a vector of doubles representing the coefficients
std::vector<double> MainWindow::parseInput(const QString &input)
{
    std::vector<double> coefficients;
    QStringList coeffList = input.split(",");
    for (const QString &coeffStr : coeffList) {
        coefficients.push_back(coeffStr.toDouble());
    }
    return coefficients;
}

// Updates the display of the transfer function based on the numerator and denominator inputs
void MainWindow::updateTransferFunctionDisplay()
{
    // Gets inputs from the user interface
    QString numeratorInput = ui->numeratorInput->text();
    QString denominatorInput = ui->denominatorInput->text();

    // Parses the inputs and creates the transfer function object
    std::vector<double> numerator = parseInput(numeratorInput);
    std::vector<double> denominator = parseInput(denominatorInput);
    TransferFunction tf(numerator, denominator);

    // Updates the labels to display the formatted numerator and denominator
    ui->numeratorLabel->setText(tf.getFormattedNumerator());
    ui->denominatorLabel->setText(tf.getFormattedDenominator());
}

// Generates and displays the bode plot for the transfer function with the specified frequency range
void MainWindow::plotBode()
{
    // Retrieves the numerator and denominator inputs
    QString numeratorInput = ui->numeratorInput->text();
    QString denominatorInput = ui->denominatorInput->text();
    std::vector<double> numerator = parseInput(numeratorInput);
    std::vector<double> denominator = parseInput(denominatorInput);

    // Gets and validates the frequency range from the user input
    bool okMin, okMax;
    double xMin = ui->minFrequencyInput->text().toDouble(&okMin);
    double xMax = ui->maxFrequencyInput->text().toDouble(&okMax);

    // Adjusts the frequency values to account for range correction (divided by 10)
    xMin /= 10;
    xMax /= 10;

    // Checks for a valid frequency range
    if (!(okMin && okMax && xMin < xMax)) {
        QMessageBox::warning(this, "Falsche Eingabe", "Bitte einen gültigen Frequenzbereich eingeben.");
        return;
    }

    // Creates the transfer function and calculates the bode plot data
    TransferFunction tf(numerator, denominator);
    std::vector<double> frequencies, magnitude, phase;
    tf.bodeData(frequencies, magnitude, phase, xMin, xMax, 500);

    // Plots the bode plot with the generated data
    BodePlot bodePlot(ui->magnitudePlot, ui->phasePlot);
    bodePlot.plot(frequencies, magnitude, phase, xMin, xMax);

    // Calculates and displays the phase margin and gain margin
    double phaseMargin = tf.calculatePhaseMargin();
    double gainMargin = tf.calculateGainMargin();

    // Displays the phase margin, handling infinity as a special case
    if (std::isinf(phaseMargin)) {
        ui->phaseMarginLabel->setText("unendlich");
    } else {
        ui->phaseMarginLabel->setText(QString::number(phaseMargin, 'f', 2) + "°");
    }

    // Displays the gain margin, handling infinity as a special case
    if (std::isinf(gainMargin)) {
        ui->gainMarginLabel->setText("unendlich");
    } else {
        ui->gainMarginLabel->setText(QString::number(gainMargin, 'f', 2) + " dB");
    }

    // Determines the system stability based on the calculated margins and displays the result
    if (phaseMargin > 0 && gainMargin > 0) {
        ui->stabilityLabel->setText("stabil");
    } else if (std::isinf(phaseMargin) && gainMargin > 0) {
        if (std::all_of(magnitude.begin(), magnitude.end(), [](double mag) { return mag < 0; })) {
            ui->stabilityLabel->setText("stabil");
        } else if (std::all_of(magnitude.begin(), magnitude.end(), [](double mag) { return mag > 0; })) {
            ui->stabilityLabel->setText("möglicherweise instabil");
        }
    } else if (std::isinf(gainMargin) && phaseMargin > 0) {
        ui->stabilityLabel->setText("stabil");
    } else if (std::isinf(gainMargin) && std::isinf(phaseMargin)) {
        ui->stabilityLabel->setText("stabil");
    } else if (phaseMargin == 0 || gainMargin == 0) {
        ui->stabilityLabel->setText("grenzstabil");
    } else {
        ui->stabilityLabel->setText("instabil");
    }
}

// Opens a file dialog to export the current bode plot in the selected format
void MainWindow::onExportButtonClicked()
{
    // Gets the selected format (PNG, JPG, PDF, or SVG) from the combo box
    QString selectedFormat = ui->exportComboBox->currentText().toLower();

    // Opens a file dialog to specify the file name and location and if the user cancels the export, exits the function
    QString fileName = QFileDialog::getSaveFileName(this, "Bodediagramm exportieren", "",
                                                    selectedFormat.toUpper() + " Datei (*." + selectedFormat + ")");
    if (fileName.isEmpty()) {
        return;
    }

    // Calls the export function to save the plot in the specified format
    exporter->exportPlot(selectedFormat, fileName);
}
