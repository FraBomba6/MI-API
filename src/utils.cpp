#include "../include/utils.h"

using namespace H5;
using namespace std;

/**
 * Converts a multidimensional array to a format compatible with the GKOV estimator library.
 * @param X - The multidimensional array.
 * @param sizes - The sizes of each dimension.
 * @param dimensions - The number of dimensions.
 * @return The converted array.
 */
[[maybe_unused]] double **MIUtils::to_gkov_format(double *X, const int *sizes, int dimensions) {
    auto **X_gkov = new double *[sizes[0]];
    int flattenedSize = 1;
    for (int i = 1; i < dimensions; i++)
        flattenedSize *= sizes[i];
    for (int i = 0; i < sizes[0]; i++)
        X_gkov[i] = X + i * flattenedSize;
    return X_gkov;
}

/**
 * Computes the distribution of a discrete input.
 * @param X - The input.
 * @param sizes - The sizes of each dimension.
 * @param dimensions - The number of dimensions.
 * @return The distribution.
 */
double *MIUtils::compute_distribution(const double *X, const int *sizes, int dimensions) {
    int flattenedSize = 1;
    for (int i = 0; i < dimensions; i++)
        flattenedSize *= sizes[i];
    auto *distribution = new double[flattenedSize];
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
Trace MIUtils::read_traces(const string &filename) {
    Trace trace{};
    H5File file(filename, H5F_ACC_RDONLY);
    DataSet dataset = file.openDataSet("traces");
    DataSpace dataspace = dataset.getSpace();

    int rank = dataspace.getSimpleExtentNdims();
    trace.dims = new hsize_t[rank];
    dataspace.getSimpleExtentDims(trace.dims, nullptr);

    trace.traces = new double[trace.dims[0] * trace.dims[1]];
    dataset.read(trace.traces, PredType::NATIVE_DOUBLE);

    try {
        Attribute attribute = dataset.openAttribute("secret_key");
        attribute.read(PredType::NATIVE_INT, &trace.secret_key);
    } catch (const H5::AttributeIException &e) {
        cout << "Attribute not found" << "\n";
    }
    dataspace.close();
    dataset.close();

    dataset = file.openDataSet("pts");
    trace.pts = new double[trace.dims[0]];
    dataset.read(trace.pts, PredType::NATIVE_DOUBLE);
    dataset.close();

    return trace;
}

/**
 * Writes traces to an HDF5 file.
 * @param filename - The name of the file.
 * @param dataset_name - The name of the dataset_name.
 * @param traces - The traces.
 * @param size - The size of the traces.
 */
void MIUtils::write_traces(const string &filename, double *traces, double *pts, const uint32_t size, unsigned int secret_key) {
    hsize_t dims[2] = {static_cast<hsize_t>(size), 1};
    DataSpace dataspace(2, dims);
    H5File file(filename, H5F_ACC_TRUNC);
    DataSet dataset = file.createDataSet("pts", PredType::NATIVE_DOUBLE, dataspace);
    dataset.write(pts, PredType::NATIVE_DOUBLE);
    dataset.close();
    dataset = file.createDataSet("traces", PredType::NATIVE_DOUBLE, dataspace);
    dataset.write(traces, PredType::NATIVE_DOUBLE);
    hsize_t dim[] = {1};
    DataSpace attr_dataspace = DataSpace(1, dim);
    Attribute attribute = dataset.createAttribute("secret_key", PredType::NATIVE_INT, attr_dataspace);
    attribute.write(PredType::NATIVE_INT, &secret_key);
    attribute.close();
    dataset.close();
    file.close();
}

/**
 * Generates a filename for the traces.
 * @return The filename.
 */
std::string MIUtils::generate_filename(uint32_t n_trc) {
    char filename[100];
    strcpy(filename, "../data/traces/");
    strcat(filename, to_string(n_trc).c_str());
    strcat(filename, "_traces.h5");
    return filename;
}
