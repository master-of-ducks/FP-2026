#/**
 * @file test_gram_schmidt.cc
 * @brief Mixed-precision Gram-Schmidt test suite.
 *
 * Runs classical and modified Gram-Schmidt in mixed precisions for
 * a variety of floating-point types. Computes orthogonality and
 * reconstruction errors and prints the results.
 * Intended to test the algorithm on all precision combinations (9⁴ combinations)
 * Warning: this might take some time to compile and run and most of these combinations do not make sense to use in practice.
 */
#include "../../hdnum.hh"
#include "../../src/gram_schmidt_mixed.hh"
// #include "../../src/densematrix.hh"
#include "../../src/testmatrices.hh"

#include <array>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <tuple>

using namespace hdnum;
const std::size_t n = 20;
const double kappa = 100000.0;

#ifdef HDNUM_HAS_CPFLOAT
#ifdef HDNUM_HAS_GMP

/**
 * @brief Tuple of all precision types to test.
 *
 * The test iterates over elements of this tuple to instantiate
 * mixed-precision variants of the Gram-Schmidt routines.
 */
using PrecisionList = std::tuple<FP8, bfloat16, FP16, FP32, FP64, FP128, FP256, FP512, FP1024>;
static constexpr std::array<const char*, 9> precision_names = {{
    "FP8", "bfloat16", "FP16", "FP32", "FP64", "FP128", "FP256", "FP512", "FP1024"
}};

template <std::size_t Index>
using Precision = std::tuple_element_t<Index, PrecisionList>;

/**
 * @brief Run one combination of precisions and report errors.
 *
 * Instantiates and runs the mixed-precision classical and modified
 * Gram-Schmidt algorithms for matrix precision `I1`, inner-product
 * precision `I2`, normalization precision `I3`, and accumulation
 * precision `I4` on an `n x n` matrix.
 *
 * @tparam I1 Index into `PrecisionList` specifying the matrix type.
 * @tparam I2 Index into `PrecisionList` specifying the inner-product type.
 * @tparam I3 Index into `PrecisionList` specifying the normalization type.
 * @tparam I4 Index into `PrecisionList` specifying the accumulation type.
 * @param n Dimension of the square test matrix.
 */
template <std::size_t I1, std::size_t I2, std::size_t I3, std::size_t I4>
void run_combo(std::size_t n) {
    using T1 = Precision<I1>;
    using T2 = Precision<I2>;
    using T3 = Precision<I3>;
    using T4 = Precision<I4>;

    const DenseMatrix<T1> I = make_identity<T1>(n);

    DenseMatrix<T1> A(n,n);
    randsvd<T1>(A, static_cast<T1>(kappa));

    const DenseMatrix<T1> Q = gram_schmidt_mixed<T1, T2, T3, T4>(A);
    const DenseMatrix<T1> Qm = modified_gram_schmidt_mixed<T1, T2, T3, T4>(A);

    const DenseMatrix<T1> QtQ = Q.transpose() * Q;
    const DenseMatrix<T1> QtQm = Qm.transpose() * Qm;
    const DenseMatrix<T1> Aproj = Q * (Q.transpose() * A);
    const DenseMatrix<T1> Aprojm = Qm * (Qm.transpose() * A);
    const DenseMatrix<T1> zero(n, n);

    const double a_norm = frobenius_norm_diff<T1>(A,zero);

    const double ortho_err = frobenius_norm_diff<T1>(QtQ, I);
    const double ortho_err_m = frobenius_norm_diff<T1>(QtQm, I);
    const double recon_err = frobenius_norm_diff<T1>(A, Aproj) / a_norm;
    const double recon_err_m = frobenius_norm_diff<T1>(A, Aprojm) / a_norm;

    std::cout << "\nTesting mixed GS with precision: " << precision_names[I1]
              << ", " << precision_names[I2]
              << ", " << precision_names[I3]
              << ", " << precision_names[I4] << std::endl;
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "  Orthogonality error ||Q^T Q - I||_F" << std::endl;
    std::cout << "    classical: " << ortho_err << std::endl;
    std::cout << "    modified:  " << ortho_err_m << std::endl;
    std::cout << "  Reconstruction error ||A - Q(Q^T A)||_F / ||A||_F" << std::endl;
    std::cout << "    classical: " << recon_err << std::endl;
    std::cout << "    modified:  " << recon_err_m << std::endl;
}

/**
 * @brief Dispatch helper to call `run_combo` for a given `i4`.
 *
 * Instantiates `run_combo` with the provided template indices `I1`,
 * `I2`, `I3` and runtime selection `i4`.
 *
 * @tparam I1 Index for matrix precision.
 * @tparam I2 Index for inner-product precision.
 * @tparam I3 Index for normalization precision.
 * @param i4 Runtime index selecting accumulation precision.
 * @param n Matrix dimension passed to `run_combo`.
 */
template <std::size_t I1, std::size_t I2, std::size_t I3>
void dispatch_i4(std::size_t i4, std::size_t n) {
    switch (i4) {
        case 0: run_combo<I1, I2, I3, 0>(n); break;
        case 1: run_combo<I1, I2, I3, 1>(n); break;
        case 2: run_combo<I1, I2, I3, 2>(n); break;
        case 3: run_combo<I1, I2, I3, 3>(n); break;
        case 4: run_combo<I1, I2, I3, 4>(n); break;
        case 5: run_combo<I1, I2, I3, 5>(n); break;
        case 6: run_combo<I1, I2, I3, 6>(n); break;
        case 7: run_combo<I1, I2, I3, 7>(n); break;
        case 8: run_combo<I1, I2, I3, 8>(n); break;
    }
}

/**
 * @brief Dispatch helper to select the `i3` index then call `dispatch_i4`.
 *
 * @tparam I1 Index for matrix precision.
 * @tparam I2 Index for inner-product precision.
 * @param i3 Runtime index selecting normalization precision.
 * @param i4 Runtime index selecting accumulation precision.
 * @param n Matrix dimension forwarded to lower dispatch.
 */
template <std::size_t I1, std::size_t I2>
void dispatch_i3(std::size_t i3, std::size_t i4, std::size_t n) {
    switch (i3) {
        case 0: dispatch_i4<I1, I2, 0>(i4, n); break;
        case 1: dispatch_i4<I1, I2, 1>(i4, n); break;
        case 2: dispatch_i4<I1, I2, 2>(i4, n); break;
        case 3: dispatch_i4<I1, I2, 3>(i4, n); break;
        case 4: dispatch_i4<I1, I2, 4>(i4, n); break;
        case 5: dispatch_i4<I1, I2, 5>(i4, n); break;
        case 6: dispatch_i4<I1, I2, 6>(i4, n); break;
        case 7: dispatch_i4<I1, I2, 7>(i4, n); break;
        case 8: dispatch_i4<I1, I2, 8>(i4, n); break;
    }
}

/**
 * @brief Dispatch helper to select the `i2` index then call `dispatch_i3`.
 *
 * Note: some low-precision inner-product types are intentionally left out
 * from testing (commented cases) because they are unsuitable for accurate
 * inner products.
 *
 * @tparam I1 Index for matrix precision.
 * @param i2 Runtime index selecting inner-product precision.
 * @param i3 Runtime index selecting normalization precision.
 * @param i4 Runtime index selecting accumulation precision.
 * @param n Matrix dimension forwarded to lower dispatch.
 */
template <std::size_t I1>
void dispatch_i2(std::size_t i2, std::size_t i3, std::size_t i4, std::size_t n) {
    switch (i2) {
        case 0: dispatch_i3<I1, 0>(i3, i4, n); break;
        case 1: dispatch_i3<I1, 1>(i3, i4, n); break;
        case 2: dispatch_i3<I1, 2>(i3, i4, n); break;
        case 3: dispatch_i3<I1, 3>(i3, i4, n); break;
        case 4: dispatch_i3<I1, 4>(i3, i4, n); break;
        case 5: dispatch_i3<I1, 5>(i3, i4, n); break;
        case 6: dispatch_i3<I1, 6>(i3, i4, n); break;
        case 7: dispatch_i3<I1, 7>(i3, i4, n); break;
        case 8: dispatch_i3<I1, 8>(i3, i4, n); break;
    }
}

/**
 * @brief Top-level dispatch selecting the matrix precision index `i1`.
 *
 * This function selects which matrix precision to instantiate and then
 * forwards the remaining indices to `dispatch_i2`.
 *
 * @param i1 Runtime index selecting matrix precision.
 * @param i2 Runtime index selecting inner-product precision.
 * @param i3 Runtime index selecting normalization precision.
 * @param i4 Runtime index selecting accumulation precision.
 * @param n Matrix dimension forwarded to lower dispatch.
 */
void dispatch_i1(std::size_t i1, std::size_t i2, std::size_t i3, std::size_t i4, std::size_t n) {
    switch (i1) {
        case 0: dispatch_i2<0>(i2, i3, i4, n); break;
        case 1: dispatch_i2<1>(i2, i3, i4, n); break;
        case 2: dispatch_i2<2>(i2, i3, i4, n); break;
        case 3: dispatch_i2<3>(i2, i3, i4, n); break;
        case 4: dispatch_i2<4>(i2, i3, i4, n); break;
        case 5: dispatch_i2<5>(i2, i3, i4, n); break;
        case 6: dispatch_i2<6>(i2, i3, i4, n); break;
        case 7: dispatch_i2<7>(i2, i3, i4, n); break;
        case 8: dispatch_i2<8>(i2, i3, i4, n); break;
    }
}

/**
 * @brief Entry point for the mixed-precision Gram-Schmidt test.
 *
 * Constructs a test matrix of fixed size and iterates over
 * all precision combinations, invoking the dispatch machinery to
 * instantiate and run tests for each combination.
 *
 * @return Exit code (0 on success).
 */
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "Mixed Precision Gram-Schmidt Test Suite" << std::endl;
    std::cout << "Testing all 9x9x9x9 type combinations for classic and modified GS." << std::endl;
    std::cout << "========================================" << std::endl;

    for (std::size_t i = 0; i < 9; ++i) {
        for (std::size_t j = 0; j < 9; ++j) {
            for (std::size_t k = 0; k < 9; ++k) {
                for (std::size_t l = 0; l < 9; ++l) {
                    dispatch_i1(i, j, k, l, n);
                }
            }
        }
    }

    return 0;
}

#else

/**
 * @brief Fallback main when mixed-precision types are unavailable.
 *
 * Prints an informative message and exits.
 */
int main() {
    std::cout << "Mixed-precision types are not enabled in this build." << std::endl;
    return 0;
}

#endif
#endif