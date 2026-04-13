#include "randomGenerator.hpp"

// a = min, b = max
double getUniforme(double a, double b) {
    double rand = getRandom();
    
    return a + (b - a) * rand;
}