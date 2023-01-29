#ifndef SMALL_PROJECT_LIBRARY_H
#define SMALL_PROJECT_LIBRARY_H

#include <armadillo>

using namespace arma;
using namespace std;

void hello();

class GKOVEstimator {
    public:
        explicit GKOVEstimator(double (*callback)(int));
    private:
        double (*t_n_)(int);
        double t_n(int n);
        static mat prepare_data(double* X, double** Y, int sizeOfX, pair<int, int> sizeOfY);
        static bool check_dimensions(int sizeOfX, pair<int, int> sizeOfY);
};

#endif
