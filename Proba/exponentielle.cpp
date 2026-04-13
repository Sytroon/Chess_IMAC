#include <cmath>
#include "randomGenerator.hpp"

double getExponentielle(double lambda) {
    if (lambda <= 0) {
        return 0.0;
    }

    double rand = getRandom();
 
    return - (std::log(rand) / lambda);
}