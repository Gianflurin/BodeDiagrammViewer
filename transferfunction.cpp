#include "transferfunction.h"
#include <complex>
#include <cmath>
#include <limits>

// Evaluates the transfer function H(jw) at a given frequency w in rad/s and represent j * w in the complex plane
std::complex<double> TransferFunction::evaluate(double w)
{
    std::complex<double> jw(0, w);
    std::complex<double> num = 0.0;
    std::complex<double> den = 0.0;

    // Computes the numerator polynomial at jw
    for (size_t i = 0; i < numerator.size(); ++i)
    {
        num += numerator[i] * std::pow(jw, numerator.size() - i - 1);
    }

    // Computes the denominator polynomial at jw
    for (size_t i = 0; i < denominator.size(); ++i)
    {
        den += denominator[i] * std::pow(jw, denominator.size() - i - 1);
    }

    // Returns the transfer function H(jw) = numerator(jw) / denominator(jw)
    return num / den;
}

// Generates the bode plot data with frequency in rad/s, magnitude in dB and phase in °
void TransferFunction::bodeData(std::vector<double> &frequencies, std::vector<double> &magnitude, std::vector<double> &phase, double freqStart, double freqEnd, int numPoints)
{
    frequencies.resize(numPoints);
    magnitude.resize(numPoints);
    phase.resize(numPoints);

    double logStart = std::log10(freqStart);
    double logEnd = std::log10(freqEnd);
    double lastUnwrappedPhase = 0.0;

    for (int i = 0; i < numPoints; ++i)
    {
        // Computes the current frequency in rad/s in a logarithmic scale and converts to linear frequency
        double logFreq = logStart + (logEnd - logStart) * i / (numPoints - 1);
        double w = std::pow(10, logFreq);
        frequencies[i] = w;

        // Evaluates the transfer function H(jw)
        std::complex<double> H = evaluate(w);

        // Computes the magnitude in dB
        magnitude[i] = 20 * std::log10(std::abs(H));

        // Computes the phase in ° and unwraps phase jumps so that the phase reaches values of more than 180° and less than -180°
        double unwrappedPhase = std::arg(H) * 180 / M_PI;
        if (i > 0) {
            double phaseDifference = unwrappedPhase - lastUnwrappedPhase;
            if (phaseDifference > 180) {
                unwrappedPhase -= 360 * std::ceil((phaseDifference - 180) / 360);
            } else if (phaseDifference < -180) {
                unwrappedPhase += 360 * std::ceil((std::abs(phaseDifference) - 180) / 360);
            }
        }

        phase[i] = unwrappedPhase;
        lastUnwrappedPhase = unwrappedPhase;
    }
}

// Formats the numerator as a string for display and returns a warning if the numerator is empty
QString TransferFunction::getFormattedNumerator()
{
    if (numerator.empty()) {
        return "Ungültiger Zähler";
    }

    QString numeratorEq = "";
    int numDegree = numerator.size() - 1;

    // Formats the numerator polynomial and skips the coefficient "1" for terms with "s" in it
    for (int i = 0; i < numerator.size(); ++i) {
        if (numerator[i] == 1 && (numDegree - i) > 0) {
        } else if (numerator[i] == -1 && (numDegree - i) > 0) {
            numeratorEq += "-";
        } else {
            numeratorEq += QString::number(numerator[i]);
        }

        // Adds the variable part "s" and its exponent, if necessary
        if (numDegree - i > 0) {
            numeratorEq += "s";
            if (numDegree - i > 1) {
                numeratorEq += "<sup>" + QString::number(numDegree - i) + "</sup>";
            }
        }

        // Adds "+" or "-" between the terms
        if (i < numerator.size() - 1) {
            numeratorEq += (numerator[i + 1] >= 0) ? " + " : " - ";
            numerator[i + 1] = std::abs(numerator[i + 1]);
        }
    }

    return numeratorEq;
}

// Formats the denominator as a string for display and returns a warning if the denominator is empty
QString TransferFunction::getFormattedDenominator()
{
    if (denominator.empty()) {
        return "Ungültiger Nenner";
    }

    QString denominatorEq = "";
    int denDegree = denominator.size() - 1;

    // Formats the denominator polynomial and skips the coefficient "1" for terms with "s" in it
    for (int i = 0; i < denominator.size(); ++i) {
        if (denominator[i] == 1 && (denDegree - i) > 0) {
        } else if (denominator[i] == -1 && (denDegree - i) > 0) {
            denominatorEq += "-";
        } else {
            denominatorEq += QString::number(denominator[i]);
        }

        // Adds the variable part "s" and its exponent, if necessary
        if (denDegree - i > 0) {
            denominatorEq += "s";
            if (denDegree - i > 1) {
                denominatorEq += "<sup>" + QString::number(denDegree - i) + "</sup>";
            }
        }

        // Adds "+" or "-" between the terms
        if (i < denominator.size() - 1) {
            denominatorEq += (denominator[i + 1] >= 0) ? " + " : " - ";
            denominator[i + 1] = std::abs(denominator[i + 1]);
        }
    }

    return denominatorEq;
}

// Calculates the gain margin of the system
double TransferFunction::calculateGainMargin()
{
    // Searches for a phase crossover (where the phase crosses -180°) in a given frequency range and with a given number of samples
    double freqStart = 10e-3;
    double freqEnd = 10e6;
    int numPoints = 1000000;

    // Initializes as no phase crossover found and a magnitude of 0 at the phase crossover
    double phaseCrossoverFreq = -1;
    double magnitudeAtPhaseCrossover = 0;

    double logStart = std::log10(freqStart);
    double logEnd = std::log10(freqEnd);

    // Sets the tolerance for detecting the -180° phase
    const double tolerance = 1e-3;
    double lastUnwrappedPhase = 0.0;

    // Loops through logarithmically spaced frequencies and converts back to a linear frequency
    for (int i = 0; i < numPoints; ++i)
    {
        double logFreq = logStart + (logEnd - logStart) * i / (numPoints - 1);
        double w = std::pow(10, logFreq);

        // Evaluates the transfer function H(jw) at the current frequency
        std::complex<double> H = evaluate(w);

        // Calculates the phase and unwraps large phase jumps
        double unwrappedPhase = std::arg(H) * 180 / M_PI;
        if (i > 0) {
            double phaseDifference = unwrappedPhase - lastUnwrappedPhase;
            if (phaseDifference > 180) {
                unwrappedPhase -= 360 * std::ceil((phaseDifference - 180) / 360);
            } else if (phaseDifference < -180) {
                unwrappedPhase += 360 * std::ceil((std::abs(phaseDifference) - 180) / 360);
            }
        }
        lastUnwrappedPhase = unwrappedPhase;

        // Checks if the phase crosses -180° within the given tolerance and records the frequency and magnitude at the phase crossover
        if (phaseCrossoverFreq < 0 && std::abs(unwrappedPhase + 180) < tolerance)
        {
            phaseCrossoverFreq = w;
            magnitudeAtPhaseCrossover = 20 * std::log10(std::abs(H));
        }

        // Exits the loop once the phase crossover is found
        if (phaseCrossoverFreq > 0) break;
    }

    // Calculates and returns the gain margin or returns 'infinity' if no phase crossover is found
    if (phaseCrossoverFreq > 0) {
        return -magnitudeAtPhaseCrossover;
    } else {
        return std::numeric_limits<double>::infinity();
    }
}

// Calculates the phase margin of the system
double TransferFunction::calculatePhaseMargin()
{
    // Searches for a gain crossover (where the magnitude crosses 0 dB) in a given frequency range and with a given number of samples
    double freqStart = 10e-3;
    double freqEnd = 10e6;
    int numPoints = 1000000;

    // Initializes as no gain crossover found and a phase of 0 at the gain crossover
    double gainCrossoverFreq = -1;
    double phaseAtGainCrossover = 0;

    double logStart = std::log10(freqStart);
    double logEnd = std::log10(freqEnd);

    // Sets the tolerance for detecting the 0 dB gain
    const double tolerance = 1e-3;
    double lastUnwrappedPhase = 0.0;

    // Loops through logarithmically spaced frequencies and converts to a linear frequnecy
    for (int i = 0; i < numPoints; ++i)
    {
        double logFreq = logStart + (logEnd - logStart) * i / (numPoints - 1);
        double w = std::pow(10, logFreq);

        // Evaluates the transfer function H(jw) at the current frequency
        std::complex<double> H = evaluate(w);

        // Calculates the magnitude and phase
        double magnitude = 20 * std::log10(std::abs(H));
        double unwrappedPhase = std::arg(H) * 180 / M_PI;

        // Unwraps the phase if there's a jump greater than ±180°
        if (i > 0) {
            double phaseDifference = unwrappedPhase - lastUnwrappedPhase;
            if (phaseDifference > 180) {
                unwrappedPhase -= 360 * std::ceil((phaseDifference - 180) / 360);
            } else if (phaseDifference < -180) {
                unwrappedPhase += 360 * std::ceil((std::abs(phaseDifference) - 180) / 360);
            }
        }
        lastUnwrappedPhase = unwrappedPhase;

        // Checks if the magnitude crosses 0 dB within the given tolerance and records the frequeny and the phase at the gain crossover
        if (gainCrossoverFreq < 0 && std::abs(magnitude) < tolerance)
        {
            gainCrossoverFreq = w;
            phaseAtGainCrossover = unwrappedPhase;
        }

        // Exits the loop once the gain crossover is found
        if (gainCrossoverFreq > 0) break;
    }

    // Calculates and return the phase margin or returns 'infinity' if no phase crossover is found
    if (gainCrossoverFreq > 0) {
        return 180 + phaseAtGainCrossover;
    } else {
        return std::numeric_limits<double>::infinity();
    }
}
