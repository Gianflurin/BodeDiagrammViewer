#include "bodeplot.h"
#include <QSharedPointer>

// Constructor for the BodePlot class, initializes magnitude and phase plots with logarithmic x-axes
BodePlot::BodePlot(QCustomPlot *magnitudePlot, QCustomPlot *phasePlot)
    : magnitudePlot(magnitudePlot), phasePlot(phasePlot)
{
    // Sets up a logarithmic ticker for the x-axis of the magnitude plot
    QSharedPointer<QCPAxisTickerLog> logTickerMag(new QCPAxisTickerLog);
    magnitudePlot->xAxis->setTicker(logTickerMag);

    // Sets up a logarithmic ticker for the x-axis of the phase plot
    QSharedPointer<QCPAxisTickerLog> logTickerPhase(new QCPAxisTickerLog);
    phasePlot->xAxis->setTicker(logTickerPhase);
}

// Plots the bode plot with a separate plot for the magnitude response and the phase response
void BodePlot::plot(const std::vector<double> &frequencies, const std::vector<double> &magnitude, const std::vector<double> &phase, double xMin, double xMax)
{
    // Converts std::vector to QVector to make them compatible with QCustomPlot
    QVector<double> qFrequencies = QVector<double>(frequencies.begin(), frequencies.end());
    QVector<double> qMagnitude = QVector<double>(magnitude.begin(), magnitude.end());
    QVector<double> qPhase = QVector<double>(phase.begin(), phase.end());

    // Clears previous data from the magnitude and phase plots
    magnitudePlot->clearGraphs();
    phasePlot->clearGraphs();

    // Plots the magnitude data and sets the axis labels
    magnitudePlot->addGraph();
    magnitudePlot->graph(0)->setData(qFrequencies, qMagnitude);
    magnitudePlot->xAxis->setLabel("Frequenz in rad/s");
    magnitudePlot->yAxis->setLabel("Amplitude in dB");

    // Sets the x-axis to logarithmic scale and defines the range using user input
    magnitudePlot->xAxis->setScaleType(QCPAxis::stLogarithmic);
    magnitudePlot->xAxis->setRange(xMin, xMax);

    // Displays the x-axis in scientific notation with powers of 10
    magnitudePlot->xAxis->setNumberFormat("eb");
    magnitudePlot->xAxis->setNumberPrecision(0);

    // Rescales only the y-axis to fit the data and replots the magnitude plot
    magnitudePlot->yAxis->rescale();
    magnitudePlot->replot();

    // Plots the phase data and sets the axis labels
    phasePlot->addGraph();
    phasePlot->graph(0)->setData(qFrequencies, qPhase);
    phasePlot->xAxis->setLabel("Frequenz in rad/s");
    phasePlot->yAxis->setLabel("Phase in °");

    // Sets the x-axis to logarithmic scale and defines the range using user input
    phasePlot->xAxis->setScaleType(QCPAxis::stLogarithmic);
    phasePlot->xAxis->setRange(xMin, xMax);

    // Displays the x-axis in scientific notation with powers of 10
    phasePlot->xAxis->setNumberFormat("eb");
    phasePlot->xAxis->setNumberPrecision(0);

    // Rescales only the y-axis to fit the data and replots the phase plot
    phasePlot->yAxis->rescale();
    phasePlot->replot();
}
