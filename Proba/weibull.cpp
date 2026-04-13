#include "randomGenerator.hpp"
#include <cmath>

/**
 * Simule une loi de Weibull
 * @param k : paramètre de forme (shape) k > 0
 * @param lambda : paramètre d'échelle (scale) lambda > 0
 */
double getWeibull(double k, double lambda) {
    if (k <= 0 || lambda <= 0) return 0.0;

    double u = getRandom(); // Notre uniforme [0, 1]

    // Formule d'inversion de la fonction de répartition :
    // X = lambda * (-ln(U))^(1/k)
    return lambda * std::pow(-std::log(u), 1.0 / k);
}