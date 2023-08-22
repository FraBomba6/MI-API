#ifndef GKOV_H
#define GKOV_H

#include <mlpack/core.hpp>
#include <mlpack/methods/neighbor_search/neighbor_search.hpp>
#include <boost/math/special_functions/digamma.hpp>

using namespace arma;
using namespace std;
using namespace mlpack;

class GKOVEstimator {
public:
    explicit GKOVEstimator(double (*callback)(int));

    double estimate(double *X, double **Y, int sizeOfX, int sizeOfY[2]);

private:
    double (*t_n_)(int);

    double t_n(int n);

    static mat prepare_data(double *X, double **Y, int sizeOfX, int sizeOfY[2]);

    static void check_dimensions(int sizeOfX, const int sizeOfY[2]);

    static NeighborSearch<NearestNeighborSort, ChebyshevDistance, mat, BallTree> prepare_search(mat data);
};

#endif
