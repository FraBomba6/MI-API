#ifndef HIST_H
#define HIST_H

#include <utility>

using namespace std;

class HistEstimator {
public:
    explicit HistEstimator(int dimensions ,int bins[dimensions], double rangeMin, double rangeMax);
    double estimate(double* X, int dimensions, int size, const int sizes[dimensions]);
private:
    int histDimensions;
    int* bins;
    int numberOfBins;
    pair<double, double> ranges;
    int sizeOfHistogram;
    pair<double, double>* rangesPerDimension;
    void generate_uniform_ranges();
    int* build_histogram(double* X, int dimensions, int size, const int sizes[dimensions]) const;
    int get_index(int* indexes);
    static void check_dimensions(int dimensions, int size, const int sizes[dimensions]);
};

#endif
