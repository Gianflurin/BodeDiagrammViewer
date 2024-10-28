#ifndef BODEPLOT_H
#define BODEPLOT_H

#include "qcustomplot.h"
#include <vector>

// The BodePlot class is responsible for displaying the bode plots including magnitude and phase responses on designated QCustomPlot objects
class BodePlot
{
public:
    // Initializes the BodePlot with pointers to QCustomPlot objects for magnitude and phase plots
    BodePlot(QCustomPlot *magnitudePlot, QCustomPlot *phasePlot);

    // Creats the bode plots using frequency, magnitude and phase data over a specified range
    void plot(const std::vector<double> &frequencies, const std::vector<double> &magnitude,
              const std::vector<double> &phase, double xMin, double xMax);

private:
    // Plots the magnitude response and the phase response
    QCustomPlot *magnitudePlot;
    QCustomPlot *phasePlot;
};

#endif
