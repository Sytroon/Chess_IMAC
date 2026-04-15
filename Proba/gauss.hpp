#pragma once
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
double Gauss(double mu, double sigma);