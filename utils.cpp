#include "utils.h"

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
double** MIUtils::to_gkov_format(double *X, int* sizes, int dimensions) {
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