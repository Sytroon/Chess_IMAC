#include <iostream>
#include <random>
#include <chrono>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Random number [0,1] generator
inline double getRandom()
{ 
    static std::mt19937 generator(std::chrono::system_clock::now().time_since_epoch().count());
    static std::uniform_real_distribution<double> distrib(0.0, 1.0);
    return distrib(generator);
}


// Loi uniforme
// a = min, b = max
double getUniforme(double a, double b) {
    double rand = getRandom();
    
    return a + (b - a) * rand;
}

// Loi géométrique
// p = probabilité de succès à chaque essai (0 < p <= 1)
// Retourne le nombre d'échec avant le premier succès
int getGeometrique(double p) {
    if (p <= 0.0) return 0; 
    if (p >= 1.0) return 0;

    double u = getRandom();

    return static_cast<int>(std::floor(std::log(u) / std::log(1.0 - p)));
}

// Loi binomiale
// n = nombre d'essai, p = probabilité de résussite
// Retourne le nombre de succès
int getBinomiale(int n, double p) {
    if (p <= 0.0) return 0;
    if (p >= 1.0) return n;

    int succes = 0;
    for (int i = 0; i < n; ++i) {
        if (getRandom() < p) {
            succes++;
        }
    }
    return succes;
}

// Loi exponentielle
// lambda augmente = courbure augmente
double getExponentielle(double lambda) {
    if (lambda <= 0) {
        return 0.0;
    }

    double rand = getRandom();
    return - (std::log(rand) / lambda);
}

// Loi de Gauss (Box-Muller)
// mu = offset, sigma = hauteur
double getGauss(double mu, double sigma) {
    // On génère deux nombres uniformes sur [0, 1]
    double u1 = getRandom();
    double u2 = getRandom();

    // Formule de Box-Muller pour obtenir une loi normale centrée réduite N(0,1)
    // On utilise cos ou sin (les deux fonctionnent)
    double z0 = std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * M_PI * u2);

    // Transformation pour obtenir N(mu, sigma^2)
    return abs(z0 * sigma + mu);
}

// Loi de Cauchy
// paramètre de position (médiane)
// paramètre d'échelle (largeur à mi-hauteur)
double getCauchy(double x0, double gamma) {
    double u = getRandom();
    // Formule d'inversion : x = x0 + gamma * tan(pi * (u - 0.5))
    return x0 + gamma * std::tan(M_PI * (u - 0.5));
}

// Loi de Poisson (méthode de Knuth)
// lambda augmente = transition expo -> Gauss
int getPoisson(double lambda) {
    if (lambda <= 0) return 0;

    // L = seuil
    double L = std::exp(-lambda);
    int k = 0;
    double p = 1.0;

    do {
        k++;
        p *= getRandom();
    } while (p > L);

    return k - 1;
}

// Loi de Weibull
// lambda = scale, k = width
double getWeibull(double k, double lambda) {
    if (k <= 0 || lambda <= 0) return 0.0;

    double u = getRandom();

    return lambda * std::pow(-std::log(u), 1.0 / k);
}