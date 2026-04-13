#include "uniforme.cpp"
#include "exponentielle.cpp"
#include "gauss.cpp"
#include "cauchy.cpp"
#include "weibull.cpp"
#include "binomiale.cpp"
#include "poisson.cpp"
#include "geometrique.cpp"


int main () {

    // Loi uniforme
    double uniforme = getUniforme(1, 8);
    std::cout << "Uniforme : " << uniforme << std::endl;
    
    // Loi exponentielle
    double exponentielle = getExponentielle(1);
    std::cout << "Exponentielle : " << exponentielle << std::endl;

    // Loi normale
    double gauss = Gauss(1, 2);
    std::cout << "Gaussienne : " << gauss << std::endl;

    // Loi de Cauchy
    double cauchy = getCauchy(1, 2);
    std::cout << "Cauchy : " << cauchy << std::endl;

    // Loi de Weibull
    double weibull = getWeibull(5, 6.2);
    std::cout << "Weibull : " << weibull << std::endl;

    // Loi binomiale
    double binomiale = getBinomiale(10, 0.7);
    std::cout << "Binomiale : " << binomiale << std::endl;

    // Loi de Poisson
    double poisson = getPoisson(4.5);
    std::cout << "Poisson : " << poisson << std::endl;

    // Loi geometrique
    double geometrique = getGeometrique(0.5);
    std::cout << "Geometrique : " << geometrique << std::endl;

    
    return 0;
}