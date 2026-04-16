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

/**
 * Simule une loi Géométrique G(p)
 * @param p : probabilité de succès à chaque essai (0 < p <= 1)
 * @return le nombre d'échecs AVANT le premier succès
 */
int getGeometrique(double p) {
    if (p <= 0.0) return 0; // Éviter la division par zéro
    if (p >= 1.0) return 0; // Succès immédiat

    double u = getRandom();

    // Formule issue de l'inversion de la fonction de répartition :
    // On prend la partie entière du logarithme
    // X = floor( ln(U) / ln(1 - p) )
    return static_cast<int>(std::floor(std::log(u) / std::log(1.0 - p)));
}

/**
 * Simule une loi Binomiale B(n, p)
 * @param n : nombre d'essais (entier >= 0)
 * @param p : probabilité de succès (0 <= p <= 1)
 * @return le nombre de succès (un entier entre 0 et n)
 */
int getBinomiale(int n, double p) {
    if (p <= 0.0) return 0;
    if (p >= 1.0) return n;

    int succes = 0;
    for (int i = 0; i < n; ++i) {
        // On tire un nombre entre 0 et 1
        // Si ce nombre est inférieur à p, c'est un succès !
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

/**
 * Simule une loi Normale par la méthode de Box-Muller
 * @param mu : Espérance (moyenne souhaitée)
 * @param sigma : Écart-type (dispersion)
 */
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

/**
 * Simule une loi de Poisson P(lambda) par l'algorithme de Knuth
 * @param lambda : la moyenne d'événements (doit être > 0)
 * @return un entier (le nombre d'événements observés)
 */
int getPoisson(double lambda) {
    if (lambda <= 0) return 0;

    // L est la valeur seuil
    double L = std::exp(-lambda);
    int k = 0;
    double p = 1.0;

    do {
        k++;
        // On multiplie par une uniforme [0, 1] à chaque itération
        p *= getRandom();
    } while (p > L);

    return k - 1;
}

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

// int main () {

//     // Loi uniforme
//     double uniforme = getUniforme(1, 8);
//     std::cout << "Uniforme : " << uniforme << std::endl;
    
//     // Loi exponentielle
//     double exponentielle = getExponentielle(1);
//     std::cout << "Exponentielle : " << exponentielle << std::endl;

//     // Loi normale
//     double gauss = Gauss(1, 2);
//     std::cout << "Gaussienne : " << gauss << std::endl;

//     // Loi de Cauchy
//     double cauchy = getCauchy(1, 2);
//     std::cout << "Cauchy : " << cauchy << std::endl;

//     // Loi de Weibull
//     double weibull = getWeibull(5, 6.2);
//     std::cout << "Weibull : " << weibull << std::endl;

//     // Loi binomiale
//     double binomiale = getBinomiale(10, 0.7);
//     std::cout << "Binomiale : " << binomiale << std::endl;

//     // Loi de Poisson
//     double poisson = getPoisson(4.5);
//     std::cout << "Poisson : " << poisson << std::endl;

//     // Loi geometrique
//     double geometrique = getGeometrique(0.5);
//     std::cout << "Geometrique : " << geometrique << std::endl;

    
//     return 0;
// }