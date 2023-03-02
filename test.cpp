#include <cmath>
#include <cstdio>
#include <random>
#include "gkov.h"

int main() {
    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<> dist(0, 1);

    GKOVEstimator estimator(log);
    double x[5] = {1, 2, 3, 4, 5};
    int sizeOfY[3] = {5, 5, 5};
    auto*** y = new double**[sizeOfY[0]];
    for (int i = 0; i < sizeOfY[0]; i++) {
        y[i] = new double*[sizeOfY[1]];
        for (int j = 0; j < sizeOfY[1]; j++) {
            y[i][j] = new double[sizeOfY[2]];
            for (int k = 0; k < sizeOfY[2]; k++)
                y[i][j][k] = dist(e2);
        }
    }
    double estimation = estimator.estimate(x, y, 5, sizeOfY);
    printf("Estimation: %f", estimation);
    return 0;
}