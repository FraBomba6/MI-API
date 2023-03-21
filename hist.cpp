#include "hist.h"
#include <stdexcept>

HistEstimator::HistEstimator(int dimensions ,int bins[dimensions], double rangeMin, double rangeMax) {
    this->histDimensions = dimensions;
    this->bins = bins;
    this->ranges.first = rangeMin;
    this->ranges.second = rangeMax;
    this->numberOfBins = 0;
    this->sizeOfHistogram = 1;
    for (int i = 0; i < dimensions; i++) {
        this->sizeOfHistogram *= this->bins[i];
        this->numberOfBins += this->bins[i];
    }
    this->rangesPerDimension = new pair<double, double>[this->numberOfBins];
    generate_uniform_ranges();
}

void HistEstimator::generate_uniform_ranges() {
    int offset = 0;
    for(int i = 0; i < this->histDimensions; i++) {
        double range = (this->ranges.second - this->ranges.first) / this->bins[i];
        for (int j = 0; j < this->bins[i]; j++) {
            this->rangesPerDimension[offset + j].first = this->ranges.first + range * j;
            this->rangesPerDimension[offset + j].second = this->ranges.first + range * (j + 1);
        }
        offset += this->bins[i];
    }
}

double HistEstimator::estimate(double* X, const int dimensions, const int size, const int sizes[dimensions]) {
    check_dimensions(dimensions, size, sizes);
    int* histogram = build_histogram(X, dimensions, size, sizes);
}

int* HistEstimator::build_histogram(double* X, const int dimensions, const int size, const int sizes[dimensions]) const {
    int* histogram = new int[this->sizeOfHistogram];
    for (int i = 0; i < this->sizeOfHistogram; i++)
        histogram[i] = 0;

    return histogram;
}

int HistEstimator::get_index(int* indexes) {
    int index = 0;
    int offset = 1;
    for (int i = this->histDimensions - 1; i >= 0; i--) {
        index += indexes[i] * offset;
        offset *= this->bins[i];
    }
    return index;
}

void HistEstimator::check_dimensions(const int dimensions, const int size, const int sizes[dimensions]) {
    if (dimensions <= 0 || size <= 0 || sizes == nullptr)
        throw invalid_argument("Dimensions, size and sizes cannot be less or equal to 0 and sizes cannot be null.");
    int sizeCheck = 1;
    for (int i = 0; i < dimensions; i++) {
        if (sizes[i] <= 0)
            throw invalid_argument("Sizes cannot be less or equal to 0.");
        sizeCheck *= sizes[i];
    }
    if (sizeCheck != size)
        throw invalid_argument("Length of array X is not equal to the product of single histogram dimensions.");
}