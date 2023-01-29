#include "library.h"
#include "mlpack/core.hpp"

using namespace arma;
using namespace std;

#include <iostream>

void hello() {
    std::cout << "Hello, World!" << std::endl;
}

GKOVEstimator::GKOVEstimator(double (*callback)(int)) {
    t_n_ = callback;
}

double GKOVEstimator::t_n(int n) {
    return t_n_(n);
}

/*
 * Prepare data for estimation.
 * @param X - X values
 * @param Y - Y values
 * @param sizeOfX - size of X
 * @param sizeOfY - size of Y
 * @return data for estimation as arma::mat
 */
mat GKOVEstimator::prepare_data(double* X, double** Y, int sizeOfX, pair<int, int> sizeOfY) {
    check_dimensions(sizeOfX, sizeOfY);
    mat data(sizeOfX, sizeOfY.second + 1);
    data.col(0) = vec(X, sizeOfX);
    for (int i = 0; i < sizeOfY.second; i++) {
        data.col(i + 1) = vec(Y[i], sizeOfX);
    }
    return data;
}

/*
 * Check if dimensions are correct.
 * @param sizeOfX - size of X
 * @param sizeOfY - size of Y
 * @return true if dimensions are correct, otherwise raise an exception
 */
bool GKOVEstimator::check_dimensions(int sizeOfX,  pair<int, int> sizeOfY) {
    if (sizeOfX <= 0) {
        throw std::invalid_argument("Size of X must be greater than 0.");
    }
    if (sizeOfY <= 0) {
        throw std::invalid_argument("Size of Y must be greater than 0.");
    }
    return sizeOfX > 0 && sizeOfY > 0;
}