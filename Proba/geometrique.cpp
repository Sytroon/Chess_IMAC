#include "randomGenerator.hpp"
#include <cmath>

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