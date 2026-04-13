#include "randomGenerator.hpp"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/**
 * Simule une loi de Cauchy par inversion de la CDF
 * @param x0 : paramètre de position (médiane)
 * @param gamma : paramètre d'échelle (largeur à mi-hauteur)
 */
double getCauchy(double x0, double gamma) {
    double u = getRandom();
    // Formule d'inversion : x = x0 + gamma * tan(pi * (u - 0.5))
    return x0 + gamma * std::tan(M_PI * (u - 0.5));
}