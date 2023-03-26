#ifndef HIST_H
#define HIST_H

#include <utility>

using namespace std;

class HistEstimator {
public:
    HistEstimator(int dimensions, int bins[dimensions], pair<double, double> ranges[dimensions]);
    double estimate(double *X, double *pX, double **Y, int size, int dimensions);
private:
    int histogramDimensions;
    int* numOfBinsPerDimension;
    int totalBins;
    int sumOfBins;
    pair<double, double>* rangesPerDimension;
    void generate_uniform_ranges(pair<double, double> *ranges);
    static double pdf_entropy(const double* pdf, int size) ;
    static pair<double *, int> unique(double* X, int size) ;
    double conditional_entropy(double *X, double *pX, double **Y, pair<int *, double *> histogram, int size);
    pair<int*, double*> build_histogram(double **Y, int size, int dimensions);
    [[nodiscard]] pair<int*, double*> initialize_histogram() const;
    void increment(int* histogram, double* values);
    int get_bin_index(double *value);
    void compute_pdf(pair<int *, double *> histogram, int size) const;
    void check_dimensions(int size, int dimensions) const;
};

#endif
