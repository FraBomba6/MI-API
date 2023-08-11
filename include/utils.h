#ifndef UTILS_H
#define UTILS_H

#include <cstdint>
#include <string>
#include <iostream>
#include <H5Cpp.h>

struct Trace {
    double *traces;
    double *pts;
    hsize_t *dims;
    unsigned int secret_key;
};

class MIUtils {
public:
    static void flatten(double *X, int *sizes, int dimensions, double *flattened, int index);

    [[maybe_unused]] static double **to_gkov_format(double *X, int *sizes, int dimensions);

    static double *compute_distribution(const double *X, const int *sizes, int dimensions);

    static Trace read_traces(const std::string &filename);

    static void
    write_traces(const std::string &filename, double *traces, double *pt, const uint32_t size,
                 unsigned int secret_key);

    static std::string generate_filename(uint32_t n_trc);
};

#endif
