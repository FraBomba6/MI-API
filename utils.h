#ifndef UTILS_H
#define UTILS_H


class MIUtils {
public:
    static void flatten(double* X, int* sizes, int dimensions, double* flattened, int index);
    static double* to_gkov_format(double *X, int* sizes, int dimensions);
};


#endif
