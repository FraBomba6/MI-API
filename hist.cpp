#include "hist.h"
#include <stdexcept>
#include <cmath>
#include <algorithm>

/**
 * Constructor for the HistEstimator class.
 * @param dimensions The number of dimensions of the histogram.
 * @param bins Array containing the number of bins per dimension.
 * @param ranges Array of ranges, one for each dimension of the histogram as pair representing min and max.
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
    this->rangesPerDimension = new pair<double, double>[this->sumOfBins];
    generate_uniform_ranges(ranges);
}

/**
 * Generates the ranges of the histogram.
 * @param ranges Array of ranges, one for each dimension of the histogram as pair representing min and max.
 */
void HistEstimator::generate_uniform_ranges(pair<double, double> *ranges) {
    int offset = 0;
    for(int i = 0; i < this->histogramDimensions; i++) {
        double range = (ranges[i].second - ranges[i].first) / this->numOfBinsPerDimension[i];
        for (int j = 0; j < this->numOfBinsPerDimension[i]; j++) {
            this->rangesPerDimension[offset + j].first = ranges[i].first + range * j;
            this->rangesPerDimension[offset + j].second = ranges[i].first + range * (j + 1);
        }
        offset += this->numOfBinsPerDimension[i];
    }
}

/**
 * Convert a multidimensional values array to a unidimensional index corresponding to a bin of the histogram.
 * @param value The multidimensional array of values.
 * @return The index of the bin to access.
 */
int HistEstimator::get_bin_index(double *value) {
    int index = 0;
    for (int i = 0; i < this->histogramDimensions; i++) {
        int offset = -1;
        for (int j = 0; j < this->numOfBinsPerDimension[i]; j++) {
            if (value[i] >= this->rangesPerDimension[j].first && value[i] < this->rangesPerDimension[j].second) {
                offset = j;
                for (int k = 0; k < i; k++)
                    offset *= this->numOfBinsPerDimension[k];
                index += offset;
            }
        }
        if (offset == -1)
            throw invalid_argument("Value out of range.");
    }
    return index;
}

/**
 * Compute the entropy of a probability density function.
 * @param pdf An array containing the probability density function.
 * @param size The size of the array.
 * @return The entropy of the probability density function.
 */
double HistEstimator::pdf_entropy(const double *pdf, int size) {
    double entropy = 0;
    for (int i = 0; i < size; i++) {
        if (pdf[i] != 0)
            entropy += pdf[i] * log(pdf[i]);
    }
    return -entropy;
}

/**
 * Compute a new array containing the unique values of the input array.
 * @param X 1D array containing size data points.
 * @param size The number of data points.
 */
pair<double*, int> HistEstimator::unique(double *X, int size) {
    auto* sorted = new double[size];
    for (int i = 0; i < size; i++)
        sorted[i] = X[i];
    sort(sorted, sorted + size);
    int uniqueSize = 0;
    for (int i = 0; i < size; i++)
        if (i == 0 || sorted[i] != sorted[i - 1])
            uniqueSize++;
    auto* unique = new double[uniqueSize];
    unique[0] = sorted[0];
    for (int i = 1; i < size; i++)
        if (sorted[i] != sorted[i - 1])
            unique[i] = sorted[i];
    return make_pair(unique, uniqueSize);
}

/**
 * Compute the conditional entropy between a discrete random variable and a continuous random variable.
 * @param histogram The histogram of the continuous random variable.
 * @param X 1D array containing size data points.
 * @param pX 1D array containing the probability of each unique value of X sorted in the same order as unique(X).
 * @param Y 2D array containing size data points where each data point is an array of values of length dimensions (one for each dimension).
 * @param size The number of data points.
 * @param dimensions The number of dimensions of the data.
 * @return The conditional entropy between X and Y.
 */
double
HistEstimator::conditional_entropy(double *X, double *pX, double **Y, pair<int *, double *> histogram, int size) {
    double entropy = 0;
    auto uniqueX = unique(X, size);
    for (int i = 0; i < uniqueX.second; i++) {
        double stepEntropy = 0;
        for (int j = 0; j < size; j++) {
            if (X[j] == uniqueX.first[i]) {
                int binIndex = get_bin_index(Y[j]);
                stepEntropy += histogram.second[binIndex] * log(histogram.second[binIndex]);
            }
        }
        entropy += pX[i] * (-stepEntropy);
    }
    return entropy;
}

/**
 * Computes the mutual information between a discrete random variable and a continuous random variable.
 * @param X 1D array containing size data points.
 * @param Y 2D array containing size data points where each data point is an array of values of length dimensions (one for each dimension).
 * @param size The number of data points.
 * @param dimensions The number of dimensions of the data.
 * @return The mutual information between X and Y.
 */
double HistEstimator::estimate(double *X, double *pX, double **Y, const int size, const int dimensions) {
    check_dimensions(size, dimensions);
    auto histogram = build_histogram(Y, size, dimensions);
    double H_Y = pdf_entropy(histogram.second, this->totalBins);
    double H_Y_given_X = conditional_entropy(X, pX, Y, histogram, size);
    return H_Y - H_Y_given_X;
}

/**
 * Initializes the histogram, composed of an array of bins and an array of probabilities.
 * @return A pair containing the histogram and the pdf.
 */
pair<int*, double*> HistEstimator::initialize_histogram() const {
    auto* histogram = new int[this->totalBins];
    auto* pdf = new double[this->totalBins];
    for (int i = 0; i < this->totalBins; i++)
        histogram[i] = 0;
    return make_pair(histogram, pdf);
}

/**
 * Increments the histogram given the values of the data point.
 * @param histogram The histogram.
 * @param values The values of the data point.
 */
void HistEstimator::increment(int *histogram, double *values) {
    int binIndex = get_bin_index(values);
    histogram[binIndex]++;
}

/**
 * Computes the pdf of the histogram.
 * @param histogram The histogram.
 */
void HistEstimator::compute_pdf(pair<int *, double *> histogram, int size) const {
    for (int i = 0; i < this->totalBins; i++)
        histogram.second[i] = (double) histogram.first[i] / size;
}

/**
 * Builds the histogram.
 * @param Y 2D array containing size data points where each data point is an array of values of length dimensions (one for each dimension).
 * @param size The number of data points.
 * @param dimensions The number of dimensions of the data.
 * @return A pair containing the histogram and the pdf.
 */
pair<int*, double*> HistEstimator::build_histogram(double **Y, const int size, const int dimensions) {
    auto histogram = initialize_histogram();
    for (int i = 0; i < size; i++)
        increment(histogram.first, Y[i]);
    compute_pdf(histogram, size);
    return histogram;
}

/**
 * Checks if the dimensions of the data are correct.
 * @param size The number of data points.
 * @param dimensions The number of dimensions of the data.
 * @throws invalid_argument if the dimensions of the data are not correct.
 * @throws invalid_argument if the dimensions of the histogram and the data do not match.
 */
void HistEstimator::check_dimensions(const int size, const int dimensions) const {
    if (dimensions <= 0 || size <= 0)
        throw invalid_argument("Dimensions and size cannot be less or equal to 0.");
    if (dimensions != this->histogramDimensions)
        throw invalid_argument("Dimensions of the histogram and the data do not match.");
}