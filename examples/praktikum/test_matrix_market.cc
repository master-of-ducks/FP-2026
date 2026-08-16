#/**
 * @file test_matrix_market.cc
 * @brief Quick test for the accuracy of the MatrixMarket read and write functions for FP1024 (only compares first entries)
 */
#include "../../hdnum.hh"
#include "../../src/gram_schmidt_mixed.hh"
#include "../../src/testmatrices.hh"

#include <array>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <tuple>
#include <fstream>
#include <filesystem>

using namespace hdnum;

// #ifdef HDNUM_HAS_CPFLOAT
// #ifdef HDNUM_HAS_GMP

int main() {
    unsigned int n = 100;
    unsigned int m = 10;
    unsigned int seed_u = 10;
    unsigned int seed_v = 11;

    // delete all files in matrix directory
    std::filesystem::path matrix_dir = "test";
    if (std::filesystem::exists(matrix_dir)) {
        for (const auto& entry : std::filesystem::directory_iterator(matrix_dir)) {
            std::filesystem::remove(entry.path());
        }
    } else {
        std::filesystem::create_directory(matrix_dir);
    }

    // Generate FP1024 matrix
    DenseMatrix<FP1024> A_kappa(n, m);
    randsvd<FP1024>(A_kappa, mixed_cast<FP1024>((double) 1), seed_u, seed_v);

    // print first entry of generated matrix
    std::cout << std::setprecision(320);
    std::cout << "print A_before[0][0]: " << A_kappa[0][0] << std::endl;

    // write generated matrix to MatrixMarket file
    std::string filename = matrix_dir.u8string() + "/test_matrix_read.mtx";
    writeMatrixMarketHighPrec(A_kappa, filename);

    // read matrix from file
    DenseMatrix<FP1024> test(n, m);
    readMatrixFromFileMatrixMarketHighPrec<FP1024>(test, filename);

    // print first entry of newly read-in matrix
    std::cout << "print A_after[0][0]: " << test[0][0] << std::endl;

    // print the difference between the two entries
    std::cout << std::setprecision(10);
    std::cout << "diff: " << test[0][0] - A_kappa[0][0] << std::endl;

    return 0;

}

// #endif
// #endif