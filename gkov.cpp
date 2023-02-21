#include "gkov.h"
#include "mlpack/methods/neighbor_search/neighbor_search.hpp"
#include <boost/math/special_functions/digamma.hpp>

using namespace arma;
using namespace std;
using namespace mlpack;
using namespace boost::math;

GKOVEstimator::GKOVEstimator(double (*callback)(int)) {
    t_n_ = callback;
}

/*
 * Estimate the Mutual Information between X and Y following the method described in https://ia.cr/2022/1201
 * @param X - X values
 * @param Y - Y values
 * @param sizeOfX - size of X
 * @param sizeOfY - size of Y
 * @return estimation of Mutual Information between X and Y
 */
double GKOVEstimator::estimate(double *X, double **Y, int sizeOfX, pair<int, int> sizeOfY) {
    size_t t = int(t_n(sizeOfX));
    mat xy_data = prepare_data(X, Y, sizeOfX, sizeOfY);
    xy_data = xy_data.t();
    mat x_data = xy_data.submat(0, 0, 0, xy_data.n_cols - 1);
    mat y_data = xy_data.submat(1, 0, xy_data.n_rows - 1, xy_data.n_cols - 1);
    NeighborSearch<NearestNeighborSort, ChebyshevDistance, mat, VPTree> xy = prepare_search(xy_data);
    NeighborSearch<NearestNeighborSort, ChebyshevDistance, mat, VPTree> x = prepare_search(x_data);
    NeighborSearch<NearestNeighborSort, ChebyshevDistance, mat, VPTree> y = prepare_search(y_data);
    Mat<size_t> neighbors;
    mat distances;
    vec d_ixy = zeros(sizeOfX);
    vec d_i = zeros(sizeOfX);
    vec n_ix = zeros(sizeOfX);
    vec n_iy = zeros(sizeOfX);
    vec a_i = zeros(sizeOfX);
    for (int i = 0; i < sizeOfX; i++) {
        xy.Search(xy_data.col(i), sizeOfX, neighbors, distances);
        d_ixy[i] = distances(t);
        if (d_ixy[i] == 0)
            d_i[i] = ((uvec)find(distances == 0)).n_elem;
        else
            d_i[i] = t;
        x.Search(x_data.col(i), sizeOfX, neighbors, distances);
        n_ix[i] = ((uvec)find(distances <= d_i[i])).n_elem;
        y.Search(y_data.col(i),sizeOfX, neighbors, distances);
        n_iy[i] = ((uvec)find(distances <= d_i[i])).n_elem;
        a_i[i] = digamma(d_i[i]) - log(n_ix[i] + 1) - log(n_iy[i] + 1);
        return sum(a_i)/sizeOfX + log(sizeOfX);
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