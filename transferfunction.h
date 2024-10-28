#ifndef TRANSFERFUNCTION_H
#define TRANSFERFUNCTION_H

#include <vector>
#include <complex>
#include <QString>

// The TransferFunction class calculates properties of a transfer function and provides data for the bode plots
class TransferFunction
{
public:
    // Initializes the transfer function with given numerator and denominator coefficients
    TransferFunction(const std::vector<double> &numerator, const std::vector<double> &denominator)
        : numerator(numerator), denominator(denominator) {}

    // Evaluates the transfer function H(jw) at the frequency w in rad/s
    std::complex<double> evaluate(double w);

    // Generates the bode plot data (frequencies, magnitude, phase) over a specified frequency range
    void bodeData(std::vector<double> &frequencies, std::vector<double> &magnitude,
                  std::vector<double> &phase, double freqStart, double freqEnd, int numPoints);

    // Calculates the gain margin of the transfer function
    double calculateGainMargin();

    // Calculates the phase margin of the transfer function
    double calculatePhaseMargin();

    // Returns the formatted numerator and denominator expressions as strings
    QString getFormattedNumerator();
    QString getFormattedDenominator();

private:
    // Creates a vector with the coefficients of the numerator polynomial and the denominator polynomial
    std::vector<double> numerator;
    std::vector<double> denominator;
};

#endif
