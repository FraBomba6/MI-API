#include "utils.h"
#include <H5Cpp.h>
#include <iostream>

using namespace H5;
using namespace std;

/**
 * Flattens a multidimensional array into a unidimensional array.
 * @param X - The multidimensional array.
 * @param sizes - The sizes of each dimension.
 * @param dimensions - The number of dimensions.
 * @param flattened - The flattened array.
 * @param index - The index of the flattened array to start writing.
 */
void MIUtils::flatten(double* X, int* sizes, int dimensions, double* flattened, int index) {
    if (dimensions == 1) {
        for (int i = 0; i < sizes[0]; i++)
            flattened[index + i] = X[i];
    } else {
        int offset = 1;
        for (int i = 1; i < dimensions; i++)
            offset *= sizes[i];
        for (int i = 0; i < sizes[0]; i++)
            flatten(X + i * offset, sizes + 1, dimensions - 1, flattened, index + i * offset);
    }
}

/**
 * Converts a multidimensional array to a format compatible with the GKOV estimator library.
 * @param X - The multidimensional array.
 * @param sizes - The sizes of each dimension.
 * @param dimensions - The number of dimensions.
 * @return The converted array.
 */
[[maybe_unused]] double** MIUtils::to_gkov_format(double *X, int* sizes, int dimensions) {
    auto** X_gkov = new double*[sizes[0]];
    int flattenedSize = 1;
    for (int i = 1; i < dimensions; i++)
        flattenedSize *= sizes[i];
    for (int i = 0; i < sizes[0]; i++) {
        auto* flattened = new double[flattenedSize];
        flatten(X + i * flattenedSize, sizes + 1, dimensions - 1, flattened, 0);
        X_gkov[i] = flattened;
    }
    return X_gkov;
}

/**
 * Computes the distribution of a discrete input.
 * @param X - The input.
 * @param sizes - The sizes of each dimension.
 * @param dimensions - The number of dimensions.
 * @return The distribution.
 */
double* MIUtils::compute_distribution(const double* X, const int* sizes, int dimensions) {
    int flattenedSize = 1;
    for (int i = 0; i < dimensions; i++)
        flattenedSize *= sizes[i];
    auto* distribution = new double[flattenedSize];
    for (int i = 0; i < flattenedSize; i++)
        distribution[i] = 0;
    for (int i = 0; i < flattenedSize; i++)
        distribution[(int) X[i]]++;
    for (int i = 0; i < flattenedSize; i++)
        distribution[i] /= flattenedSize;
    return distribution;
}

/**
 * Reads traces from an HDF5 file.
 * @param filename - The name of the file.
 * @param dataset_name - The name of the dataset_name.
 * @return The traces.
 */
double *MIUtils::read_traces(const char* filename, const char* dataset_name) {
    H5File file(filename, H5F_ACC_RDONLY);
    DataSet dataset = file.openDataSet(dataset_name);
    DataSpace dataspace = dataset.getSpace();

    // Compute the size of the dataset
    int rank = dataspace.getSimpleExtentNdims();
    auto* dims_out = new hsize_t[rank];
    dataspace.getSimpleExtentDims(dims_out, nullptr);

    // Read the dataset
    auto* traces = new double[dims_out[0] * dims_out[1]];
    dataset.read(traces, PredType::NATIVE_DOUBLE);

    return traces;
}