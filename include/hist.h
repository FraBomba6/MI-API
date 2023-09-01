#ifndef HIST_H
#define HIST_H

#include <utility>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <gsl/gsl_histogram.h>
#include <gsl/gsl_histogram2d.h>
#include <iostream>

using namespace std;

struct Histogram {
    int *histogram;
    gsl_histogram *gsl_histogram_1d;
    gsl_histogram2d *gsl_histogram_2d;
    double *pdf;
    int size;
    int dimensions;
};


class HistEstimator {
public:
    HistEstimator(int dimensions, int bins[], pair<double, double> ranges[]);

    double estimate(const double *X, const double *pX, double *Y, int size, int dimensions);

private:
    int histogramDimensions;
    int *numOfBinsPerDimension;
    int totalBins;
    int sumOfBins;
    pair<double, double> *rangesPerDimension;

    Histogram *build_histogram(double *Y, int size, int dimensions);

    [[nodiscard]] Histogram *initialize_histogram() const;

    gsl_histogram *build_1d_gsl_histogram(const double *Y, int size);

    gsl_histogram2d *build_2d_gsl_histogram(const double *Y, int size);

    int *build_nd_histogram(const double *Y, int size);

    int *get_bin_indexes(const double *value);

    void compute_pdf(Histogram *histogram) const;

    static double pdf_entropy(const double *pdf, int size) ;

    double conditional_entropy(const double *X, const double *pX, const double *Y, Histogram *histogram, int size);

    static pair<double*, int> unique(const double *X, int size);

};

#endif
