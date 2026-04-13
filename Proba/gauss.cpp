#include <cmath>
#include "randomGenerator.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/**
 * Simule une loi Normale par la méthode de Box-Muller
 * @param mu : Espérance (moyenne souhaitée)
 * @param sigma : Écart-type (dispersion)
 */
double Gauss(double mu, double sigma) {
    // On génère deux nombres uniformes sur [0, 1]
    double u1 = getRandom();
    double u2 = getRandom();

    // Formule de Box-Muller pour obtenir une loi normale centrée réduite N(0,1)
    // On utilise cos ou sin (les deux fonctionnent)
    double z0 = std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * M_PI * u2);

    // Transformation pour obtenir N(mu, sigma^2)
    return z0 * sigma + mu;
}