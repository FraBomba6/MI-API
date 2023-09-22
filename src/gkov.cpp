#include "../include/gkov.h"

using namespace arma;
using namespace std;
using namespace mlpack;
using namespace boost::math;

/**
 * Constructor for GKOVEstimator
 * @param callback function to compute t_n
 */
GKOVEstimator::GKOVEstimator(double (*callback)(int)) {
    t_n_ = callback;
}

/**
 * Estimate the Mutual Information between X and Y following the method described in https://ia.cr/2022/1201
 * @param X - X values
 * @param Y - Y values
 * @param sizeOfX - size of X
 * @param sizeOfY - size of Y
 * @return estimation of Mutual Information between X and Y
 */
double GKOVEstimator::estimate(double *X, double **Y, int sizeOfX, int sizeOfY[2]) {
    size_t t = int(t_n(sizeOfX));
    mat xy_data = prepare_data(X, Y, sizeOfX, sizeOfY);
    xy_data = xy_data.t();
    mat x_data = xy_data.submat(0, 0, 0, xy_data.n_cols - 1);
    mat y_data = xy_data.submat(1, 0, xy_data.n_rows - 1, xy_data.n_cols - 1);
    auto xy_neighbors = prepare_ball_search(xy_data);
    auto xy_distance = prepare_ball_range_search(xy_data);
    auto x = prepare_ball_range_search(x_data);
    auto y = prepare_ball_range_search(y_data);

    Mat<size_t> neighbors;
    std::vector<std::vector<size_t>> range_neighbors;
    mat distances;
    std::vector<std::vector<double>> range_distances;
    vec d_ixy = zeros(sizeOfX);
    vec d_i = zeros(sizeOfX);
    vec n_ix = zeros(sizeOfX);
    vec n_iy = zeros(sizeOfX);
    vec a_i = zeros(sizeOfX);
    for (int i = 0; i < sizeOfX; i++) {
        xy_neighbors.Search(xy_data.col(i), t + 1, neighbors, distances);
        d_ixy[i] = distances(t, 0);
    }
    for (int i = 0; i < sizeOfX; i++) {
        cout << "Estimating GKOV " << i+1 << "/" << sizeOfX  << "\r";
        if (d_ixy[i] == 0) {
            xy_distance.Search(xy_data.col(i), Range(0, 1e-15), range_neighbors, range_distances);
            d_i[i] = range_neighbors.size();
        }
        else
            d_i[i] = t;
        x.Search(x_data.col(i), Range(0, d_ixy[i]), range_neighbors, range_distances);
        for(int j = 0; j < range_neighbors.size(); j++)
            if (range_neighbors[j].size() > 0)
                n_ix[i] += range_neighbors[j].size();
        range_neighbors.clear();
        range_distances.clear();
        y.Search(y_data.col(i), Range(0, d_ixy[i]), range_neighbors, range_distances);
        for(int j = 0; j < range_neighbors.size(); j++)
            if (range_neighbors[j].size() > 0)
                n_iy[i] += range_neighbors[j].size();
        range_neighbors.clear();
        range_distances.clear();
        a_i[i] = (digamma(d_i[i]) - log(n_ix[i]) - log(n_iy[i]) + log(sizeOfX)) / sizeOfX;
    }
    cout << "\n";

    xy_data.clear();
    x_data.clear();
    y_data.clear();
    neighbors.clear();
    distances.clear();
    d_ixy.clear();
    d_i.clear();
    n_ix.clear();
    n_iy.clear();

    return sum(a_i);
}

/**
 * Computes the t_n value for the given n.
 * @param n - length of the dataset
 * @return t_n value
 */
double GKOVEstimator::t_n(int n) {
    return t_n_(n);
}

/**
 * Prepare data for estimation.
 * @param X - X values
 * @param Y - Y values
 * @param sizeOfX - size of X
 * @param sizeOfY - size of Y
 * @return data for estimation as arma::mat
 */
mat GKOVEstimator::prepare_data(double *X, double **Y, int sizeOfX, int sizeOfY[2]) {
    check_dimensions(sizeOfX, sizeOfY);
    mat data(sizeOfX, sizeOfY[1] + 1);
    data.col(0) = vec(X, sizeOfX);
    for (int i = 0; i < sizeOfY[0]; i++)
        for (int j = 0; j < sizeOfY[1]; j++)
            data(i, j + 1) = Y[i][j];
    return data;
}

/**
 * Check if histogramDimensions are correct.
 * @param sizeOfX - size of X
 * @param sizeOfY - size of Y
 * @return true if histogramDimensions are correct, otherwise raise an exception
 */
void GKOVEstimator::check_dimensions(const int sizeOfX, const int sizeOfY[2]) {
    if (sizeOfX <= 0) {
        throw std::invalid_argument("Size of X must be greater than 0.");
    }
    if (sizeOfY[0] <= 0 || sizeOfY[1] <= 0) {
        throw std::invalid_argument("Sizes of Y must be greater than 0.");
    }
    if (sizeOfX != sizeOfY[0]) {
        throw std::invalid_argument("Size of X must be equal to first size of Y.");
    }
}

NeighborSearch<NearestNeighborSort, ChebyshevDistance, mat, BallTree> GKOVEstimator::prepare_ball_search(mat data) {
    NeighborSearch<NearestNeighborSort, ChebyshevDistance, mat, BallTree> search(data);
    return search;
}

RangeSearch<ChebyshevDistance, mat, BallTree> GKOVEstimator::prepare_ball_range_search(mat data) {
    RangeSearch<ChebyshevDistance, mat, BallTree> search(data);
    return search;
}