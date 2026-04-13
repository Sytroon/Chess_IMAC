#include "randomGenerator.hpp"

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