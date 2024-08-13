
# C++ API for Mutual Information Estimation in Side-Channel Analysis

## Overview

This repository contains the implementation of a C++ API designed to estimate Mutual Information (MI) with a focus on applications in side-channel analysis for cybersecurity. The project introduces an implementation of the GKOV estimator, which is particularly suited for mixed cases involving both discrete and continuous random variables. The API also includes a traditional histogram estimator for comparative analysis.

## Project Structure

- **src/**: Contains the source code for the C++ API.
  - `GKOVEstimator.cpp`: Implementation of the GKOV estimator.
  - `HistogramEstimator.cpp`: Implementation of the histogram estimator.
  - `Simulator.cpp`: A simulator for generating side-channel leakage traces.
  - `Utils.cpp`: Utility functions for data processing and trace handling.

- **include/**: Header files for the classes and functions.

- **data/**: Contains data and results generated during project execution, including simulated traces and MI estimation results.

- **tests/**: Unit tests for the various components of the API.

- **docs/**: Documentation and additional resources.

- **CMakeLists.txt**: Configuration file for building the project.

## Key Features

### GKOV Estimator
The GKOV estimator is implemented to estimate MI between discrete and continuous variables using a nearest neighbors approach. This implementation leverages the MLPack library for efficient nearest neighbor searches.

### Histogram Estimator
The histogram estimator is used as a baseline for comparison with the GKOV estimator. It discretizes continuous variables into bins and calculates MI based on joint and marginal frequencies.

### Simulator
A simulator is provided to generate side-channel leakage traces for evaluation purposes. It supports various noise models and allows for the simulation of different cryptographic algorithms.

### Utilities
Utility functions are available for tasks such as data flattening, distribution computation, and trace handling using the HDF5 format.

## Installation

To build the project, follow these steps:

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/mi-estimator.git
   cd mi-estimator
   ```

2. Ensure that you have the necessary dependencies installed:
   - MLPack
   - GSL (GNU Scientific Library)
   - HDF5

3. Build the project using CMake:
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

## Usage

### Running the Simulator

To generate traces using the simulator:

```bash
./Simulator --output <output_file.h5> --noise gaussian --trace-length <length>
```

### Estimating Mutual Information

You can estimate MI using either the GKOV or histogram estimator:

```bash
./EstimateMI --input <input_file.h5> --estimator gkov
```

or

```bash
./EstimateMI --input <input_file.h5> --estimator histogram
```

## Project Evaluation

The project was evaluated by comparing the GKOV estimator against the histogram estimator using simulated side-channel traces. The GKOV estimator demonstrated superior accuracy, particularly as the number of samples increased, confirming its effectiveness in estimating MI for mixed-variable scenarios.

### Results

Results are provided in JSON format, with detailed metrics for each key tested. The GKOV estimator consistently outperformed the histogram estimator, particularly in scenarios with more significant data complexity.

## Conclusion

This project provides a robust API for estimating MI in cybersecurity applications, particularly in side-channel analysis. The GKOV estimator, supported by this API, offers improved accuracy over traditional methods, making it a valuable tool for researchers and practitioners in the field.

## Future Work

Future developments may include:
- Optimization of the histogram estimator parameters.
- Exploration of alternative MI estimation techniques.
- Extension of the simulator to support multi-dimensional traces and additional noise models.

## References

The implementation and methodologies are based on the works cited in the [references](./references.bib) file.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
