#include "randomGenerator.hpp"
#include <cmath>

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