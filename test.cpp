#include <cmath>
#include <cstdio>
#include "gkov.h"

int main() {
    GKOVEstimator estimator(log);
    double x[5] = {1, 2, 3, 4, 5};
    auto** y = new double*[5];
    for (int i = 0; i < 5; ++i) {
        y[i] = new double[5];
        for (int j = 0; j < 5; ++j)
            y[i][j] = i+j;
    }
    double estimation = estimator.estimate(x, y, 5, make_pair(5, 5));
    printf("Estimation: %f", estimation);
    return 0;
}