#ifndef EXPORTBODEPLOT_H
#define EXPORTBODEPLOT_H

#include <QString>
#include "qcustomplot.h"

// The ExportBodePlot class manages exporting the bode plot data to files in various formats
class ExportBodePlot
{
public:
    // Initializes ExportBodePlot with pointers to the magnitude and phase QCustomPlot objects
    ExportBodePlot(QCustomPlot *magnitudePlot, QCustomPlot *phasePlot);

    // Exports the bode plots to a specified file format and filename
    void exportPlot(const QString &format, const QString &fileName);

private:
    // Plots the magnitude response and the phase response
    QCustomPlot *magnitudePlot;
    QCustomPlot *phasePlot;
};

#endif
