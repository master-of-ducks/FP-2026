#/**
 * @file make_test_set.cc
 * @brief Generates matrices over varying condition numbers and stores them as MatrixMarket files.
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
    size_t max_kappa = 1e16;
    unsigned int step = 3;
    unsigned int seed_u = 10;
    unsigned int seed_v = 11;
    //set seeds random
    srand(time(0));
    seed_u = rand() % 1000;
    seed_v = rand() % 1000;

    seed_u = 730;
    seed_v = 169;

    // delete all files in matrix directory
    std::filesystem::path matrix_dir = "matrix";
    if (std::filesystem::exists(matrix_dir)) {
        for (const auto& entry : std::filesystem::directory_iterator(matrix_dir)) {
            std::filesystem::remove(entry.path());
        }
    } else {
        std::filesystem::create_directory(matrix_dir);
    }

    // Generate test matrices with varying condition numbers and write to files
    for (std::size_t i = 10; i < max_kappa; i*=step) {
        // Generate FP1024 matrix
        DenseMatrix<FP1024> A_kappa(n, m);
        randsvd<FP1024>(A_kappa, mixed_cast<FP1024>((double)i), seed_u, seed_v);
        std::string filename = matrix_dir.u8string() + "/matrix_kappa_" + std::to_string(i) + "_seed_u_v_" + std::to_string(seed_u)+"_"+std::to_string(seed_v) + ".mtx";
        writeMatrixMarketHighPrec(A_kappa, filename);

    }

    return 0;

}

// #endif
// #endif