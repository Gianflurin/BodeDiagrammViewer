#include "exportbodeplot.h"
#include <QSvgGenerator>
#include <QFileDialog>
#include "qcustomplot.h"

ExportBodePlot::ExportBodePlot(QCustomPlot *magnitudePlot, QCustomPlot *phasePlot)
    : magnitudePlot(magnitudePlot), phasePlot(phasePlot)
{
}

// Exports the bode plots in the specified format (PNG, JPG, PDF, or SVG)
void ExportBodePlot::exportPlot(const QString &format, const QString &fileName)
{
    QString baseFileName = fileName;

    // Removes the file extension if the filename already contains it
    if (baseFileName.endsWith("." + format)) {
        baseFileName = baseFileName.left(baseFileName.length() - format.length() - 1);
    }

    // Exports the plots based on the requested file format
    if (format == "png") {
        // Exports the plots as PNG files
        magnitudePlot->savePng(baseFileName + "_Amplitudengang.png");
        phasePlot->savePng(baseFileName + "_Phasengang.png");
    } else if (format == "jpg") {
        // Exports the plots as JPG files
        magnitudePlot->saveJpg(baseFileName + "_Amplitudengang.jpg");
        phasePlot->saveJpg(baseFileName + "_Phasengang.jpg");
    } else if (format == "pdf") {
        // Exports the plots as PDF files
        magnitudePlot->savePdf(baseFileName + "_Amplitudengang.pdf");
        phasePlot->savePdf(baseFileName + "_Phasengang.pdf");
    } else if (format == "svg") {
        // Exports the plots as a SVG file using the QSvgGenerator to render plots into vector graphics format
        QSvgGenerator generator;
        generator.setFileName(baseFileName + ".svg");

        // Combines both magnitude and phase plots vertically in one SVG canvas
        QSize combinedSize(magnitudePlot->width(), magnitudePlot->height() + phasePlot->height());
        generator.setSize(combinedSize);
        generator.setViewBox(QRect(0, 0, combinedSize.width(), combinedSize.height()));

        // Paints on the SVG generator and enables antialiasing for a smoother rendering
        QCPPainter painter;
        painter.begin(&generator);
        painter.setRenderHint(QPainter::Antialiasing);

        // Draws the magnitude plot at the top of the SVG file
        QRect magnitudeRect(0, 0, magnitudePlot->width(), magnitudePlot->height());
        magnitudePlot->toPainter(&painter, magnitudeRect.width(), magnitudeRect.height());

        // Draws the phase plot at the bottom of the SVG file
        QRect phaseRect(0, magnitudePlot->height(), phasePlot->width(), phasePlot->height());
        painter.translate(0, magnitudePlot->height());
        phasePlot->toPainter(&painter, phaseRect.width(), phaseRect.height());

        painter.end();
    }
}
