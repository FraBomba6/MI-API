#include "library.h"
#include "mlpack/core.hpp"
#include "mlpack/methods/neighbor_search/neighbor_search.hpp"

using namespace arma;
using namespace std;
using namespace mlpack;

#include <iostream>

void hello() {
    std::cout << "Hello, World!" << std::endl;
}

GKOVEstimator::GKOVEstimator(double (*callback)(int)) {
    t_n_ = callback;
}

double GKOVEstimator::estimate(double *X, double **Y, int sizeOfX, pair<int, int> sizeOfY) {
    size_t t = int(t_n_(sizeOfX));
    mat data = prepare_data(X, Y, sizeOfX, sizeOfY);
    data = data.t();
    NeighborSearch<NearestNeighborSort, ChebyshevDistance, mat, VPTree> nn = prepare_search(data);
    Mat<size_t> neighbors;
    mat distances;
    vec d_ixy = zeros(sizeOfX);
    vec d_i = zeros(sizeOfX);
    for (int i = 0; i < sizeOfX; i++) {
        mat x = data.row(i);
        nn.Search(data.col(i),sizeOfX, neighbors, distances);
        d_ixy[i] = distances(t);
        ufind(distances == 0);
    }

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
void GKOVEstimator::check_dimensions(int sizeOfX,  pair<int, int> sizeOfY) {
    if (sizeOfX <= 0) {
        throw std::invalid_argument("Size of X must be greater than 0.");
    }
    if (sizeOfY.first <= 0 || sizeOfY.second <= 0) {
        throw std::invalid_argument("Sizes of Y must be greater than 0.");
    }
    if (sizeOfX != sizeOfY.first) {
        throw std::invalid_argument("Size of X must be equal to first size of Y.");
    }
}

NeighborSearch<NearestNeighborSort, ChebyshevDistance, mat, VPTree> GKOVEstimator::prepare_search(mat data) {
    NeighborSearch<NearestNeighborSort, ChebyshevDistance, mat, VPTree> search(data);
    return search;
}