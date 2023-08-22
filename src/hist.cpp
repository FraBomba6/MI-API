#include "../include/hist.h"

/**
 * Constructor for the HistEstimator class.
 * @param dimensions - The number of dimensions of the histogram.
 * @param bins - Array containing the number of bins per dimension.
 * @param ranges - Array of ranges, one for each dimension of the histogram as pair representing min and max.
 * @return A HistEstimator object.
 */
HistEstimator::HistEstimator(int dimensions, int bins[dimensions], pair<double, double> ranges[dimensions]) {
    this->histogramDimensions = dimensions;
    this->numOfBinsPerDimension = bins;
    this->totalBins = 1;
    this->sumOfBins = 0;
    for (int i = 0; i < dimensions; i++) {
        this->totalBins *= this->numOfBinsPerDimension[i];
        this->sumOfBins += this->numOfBinsPerDimension[i];
    }
    this->rangesPerDimension = ranges;
}

/**
 * Estimates the entropy of the input.
 */
double HistEstimator::estimate(const double *X, const double *pX, double *Y, int size, int dimensions) {
    auto histogram = build_histogram(Y, size, dimensions);
    double H_Y = pdf_entropy(histogram->pdf, this->totalBins);
    double H_Y_given_X = conditional_entropy(X, pX, Y, histogram, size);
    return H_Y - H_Y_given_X;
}

/**
 * Builds a histogram and computes its pdf.
 * @param Y - The input.
 * @param size - The size of the input.
 * @param dimensions - The number of dimensions of the input.
 * @return The histogram.
 */
Histogram *HistEstimator::build_histogram(double *Y, int size, int dimensions) {
    Histogram *histogram = initialize_histogram();
    if (dimensions == 1) {
        histogram->gsl_histogram = build_1d_gsl_histogram(Y, size);
    } else if (dimensions == 2) {
        histogram->gsl_histogram2d = build_2d_gsl_histogram(Y, size);
    } else {
        histogram->histogram = build_nd_histogram(Y, size);
    }
    compute_pdf(histogram);
    return histogram;
}

/**
 * Initializes a histogram.
 * @return The initialized but empty histogram.
 */
Histogram *HistEstimator::initialize_histogram() const {
    auto *histogram = new Histogram;
    histogram->histogram = new int[this->totalBins];
    histogram->gsl_histogram = nullptr;
    histogram->gsl_histogram2d = nullptr;
    histogram->pdf = new double[this->totalBins];
    histogram->size = this->totalBins;
    histogram->dimensions = this->histogramDimensions;
    for (int i = 0; i < this->totalBins; i++) {
        histogram->histogram[i] = 0;
        histogram->pdf[i] = 0;
    }
    return histogram;
}

/**
 * Builds a 1D gsl histogram.
 * @param Y - The input.
 * @param size - The size of the input.
 * @return A pair containing the histogram and the bin centers.
 */
gsl_histogram *HistEstimator::build_1d_gsl_histogram(const double *Y, int size) {
    auto *histogram = gsl_histogram_alloc(this->numOfBinsPerDimension[0]);
    gsl_histogram_set_ranges_uniform(histogram, this->rangesPerDimension[0].first, this->rangesPerDimension[0].second);
    for (int i = 0; i < size; i++)
        gsl_histogram_increment(histogram, Y[i]);
    return histogram;
}

/**
 * Builds a 2D gsl histogram.
 * @param Y - The input.
 * @param size - The size of the input.
 * @return A pair containing the histogram and the bin centers.
 */
gsl_histogram2d *HistEstimator::build_2d_gsl_histogram(const double *Y, int size) {
    auto *histogram = gsl_histogram2d_alloc(this->numOfBinsPerDimension[0], this->numOfBinsPerDimension[1]);
    gsl_histogram2d_set_ranges_uniform(histogram, this->rangesPerDimension[0].first, this->rangesPerDimension[0].second, this->rangesPerDimension[1].first, this->rangesPerDimension[1].second);
    for (int i = 0; i < size/2; i++)
        gsl_histogram2d_increment(histogram, Y[i], Y[i + size/2]);
    return histogram;
}

/**
 * Builds an n-dimensional histogram.
 * @param Y - The input.
 * @param size - The size of the input.
 * @return A pair containing the histogram and the bin centers.
 */
int *HistEstimator::build_nd_histogram(const double *Y, int size) {
    auto *histogram = new int[this->totalBins];
    for (int i = 0; i < this->totalBins; i++)
        histogram[i] = 0;
    for (int i = 0; i < size/this->histogramDimensions; i++) {
        auto *values = new double[this->histogramDimensions];
        for (int j = 0; j < this->histogramDimensions; j++)
            values[j] = Y[i + j * size/this->histogramDimensions];
        int *bin_indexes = get_bin_indexes(values);
        for (int j = 0; j < this->histogramDimensions; j++)
            histogram[bin_indexes[j]]++;
    }
    return histogram;
}

/**
 * Computes the indexes of the bins of the histogram where the values belong.
 * @param value - The values.
 * @return The indexes of the bins.
 */
int *HistEstimator::get_bin_indexes(const double *value) {
    auto *bin_indexes = new int[this->histogramDimensions];
    int offset = 0;
    for (int i = 0; i < this->histogramDimensions; i++) {
        double range = (this->rangesPerDimension[i].second - this->rangesPerDimension[i].first) / this->numOfBinsPerDimension[i];
        bin_indexes[i] = (int) floor((value[i] - this->rangesPerDimension[i].first) / range) + offset;
        offset += this->numOfBinsPerDimension[i];
    }
    return bin_indexes;
}

/**
 * Computes the pdf of the histogram.
 * @param histogram - The histogram.
 * @return The pdf.
 */
void HistEstimator::compute_pdf(Histogram *histogram) const {
    int sum = 0;
    if (histogram->dimensions > 2) {
        for (int i = 0; i < histogram->size; i++)
            sum += histogram->histogram[i];
        for (int i = 0; i < histogram->size; i++)
            histogram->pdf[i] = (double) histogram->histogram[i] / sum;
    }
    else if (histogram->dimensions == 2) {
        for (int i = 0; i < this->numOfBinsPerDimension[0]; i++)
            for (int j = 0; j < this->numOfBinsPerDimension[1]; j++)
                sum += (int) gsl_histogram2d_get(histogram->gsl_histogram2d, i, j);
        for (int i = 0; i < this->numOfBinsPerDimension[0]; i++)
            for (int j = 0; j < this->numOfBinsPerDimension[1]; j++)
                histogram->pdf[i * this->numOfBinsPerDimension[1] + j] = gsl_histogram2d_get(histogram->gsl_histogram2d, i, j) / sum;
    }
    else {
        for (int i = 0; i < this->numOfBinsPerDimension[0]; i++)
            sum += (int) gsl_histogram_get(histogram->gsl_histogram, i);
        for (int i = 0; i < this->numOfBinsPerDimension[0]; i++)
            histogram->pdf[i] = gsl_histogram_get(histogram->gsl_histogram, i) / sum;
    }
}

/**
 * Computes the entropy of the pdf.
 * @param pdf - The pdf.
 * @param size - The size of the pdf.
 * @return The entropy.
 */
double HistEstimator::pdf_entropy(const double *pdf, int size) {
    double entropy = 0;
    for (int i = 0; i < size; i++)
        if (pdf[i] != 0)
            entropy += pdf[i] * log2(pdf[i]);
    return -entropy;
}

/**
 * Computes the conditional entropy of the pdf.
 * @param X - The discrete input.
 * @param pX - The pdf of the discrete input.
 * @param Y - The continuous input.
 * @param histogram - The histogram of the continuous input.
 * @param size - The size of the input.
 * @return The conditional entropy.
 */
double HistEstimator::conditional_entropy(const double *X, const double *pX, const double *Y, Histogram *histogram, int size) {
    double entropy = 0;
    auto uniqueX = unique(X, size);
    for (int i = 0; i < uniqueX.second; i++) {
        double stepEntropy = 0;
        for (int j = 0; j < size; j++) {
            if (X[j] == uniqueX.first[i]) {
                auto values = new double[histogram->dimensions];
                for (int k = 0; k < histogram->dimensions; k++) {
                    int index = k * size / histogram->dimensions + j;
                    values[k] = Y[index];
                }
                int *bin_indexes = get_bin_indexes(values);
                int bin_index = 0;
                int offset = 1;
                for (int k = 0; k < histogram->dimensions; k++) {
                    bin_index += bin_indexes[k] * offset;
                    offset *= this->numOfBinsPerDimension[k];
                }
                stepEntropy += histogram->pdf[bin_index] * log2(histogram->pdf[bin_index]);
            }
        }
        entropy += pX[(int) X[i]] * (-stepEntropy);
    }
    return entropy;
}

/**
 * Compute a new array containing the unique values of the input array.
 * @param X - 1D array containing size data points.
 * @param size - The number of data points.
 * @return A pair containing the unique values array and its size.
 */
pair<double *, int> HistEstimator::unique(const double *X, int size){
    auto *sorted = new double[size];
    for (int i = 0; i < size; i++)
        sorted[i] = X[i];
    sort(sorted, sorted + size);
    int uniqueSize = 0;
    for (int i = 0; i < size; i++)
        if (i == 0 || sorted[i] != sorted[i - 1])
            uniqueSize++;
    auto *unique = new double[uniqueSize];
    unique[0] = sorted[0];
    for (int i = 1; i < size; i++)
        if (sorted[i] != sorted[i - 1])
            unique[i] = sorted[i];
    return make_pair(unique, uniqueSize);
}