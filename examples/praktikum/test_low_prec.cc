#/**
 * @file test_low_prec.cc
 * @brief Test suite to find the condition number at which different low precision runs fail
 *
 * @details Previous experiments have shown, that orthogonalisations using FP8 or FP16 fail
 * relatively early. This test suite is intended to finde the precise condition number for which the different
 * implementations fail, using uniform precisions.
 */
#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

#include "../../hdnum.hh"
#include "../../src/gram_schmidt_mixed.hh"
#include "../../src/testmatrices.hh"

using namespace hdnum;

#ifdef HDNUM_HAS_CPFLOAT

namespace {

/**
 * @brief Tracks whether one Gram-Schmidt variant is still succeeding in the sweep.
 */
struct AlgorithmState {
	const char* name;
	double max_ok_kappa = 0.0;
	bool saw_success = false;
	bool active = true;
};

/**
 * @brief Run one Gram-Schmidt variant and update its state for a single kappa value.
 *
 * The callable is expected to instantiate one of the mixed-precision
 * Gram-Schmidt implementations with all four template parameters set to the
 * same target type.
 *
 * @tparam T Matrix scalar type.
 * @tparam Algorithm Callable type used to invoke the algorithm.
 * @param A Input matrix in the target precision.
 * @param state Mutable state tracking the last successful kappa.
 * @param kappa Current condition number of the test matrix.
 * @param algorithm Callable that performs the orthogonalization.
 */
template <typename T, typename Algorithm>
void try_algorithm(const DenseMatrix<T>& A,
				   AlgorithmState& state,
				   double kappa,
				   Algorithm&& algorithm) {
	if (!state.active) {
		return;
	}

	try {
		const DenseMatrix<T> Q = algorithm(A);
		(void)Q;
		state.max_ok_kappa = kappa;
		state.saw_success = true;
		std::cout << "    " << std::setw(5) << state.name << " ok" << std::endl;
	} catch (const std::exception& e) {
		state.active = false;
		std::cout << "    " << std::setw(5) << state.name << " failed at kappa="
				  << std::scientific << std::setprecision(3) << kappa
				  << " (" << e.what() << ")" << std::endl;
	} catch (...) {
		state.active = false;
		std::cout << "    " << std::setw(5) << state.name << " failed at kappa="
				  << std::scientific << std::setprecision(3) << kappa
				  << " (unknown exception)" << std::endl;
	}
}

/**
 * @brief Sweep increasing kappa values for one target precision and report failures.
 *
 * A square test matrix is generated in double precision for each kappa, cast to
 * the requested low-precision type, and then passed to CGS, MGS, CGS2, and MGS2.
 * The sweep stops once all four variants have failed at least once.
 *
 * @tparam T Target scalar type used for the tested matrix and all four template parameters.
 * @param precision_name Human-readable name printed in the report.
 */
template <typename T>
void run_precision_suite(const char* precision_name) {
	std::vector<double> kappas;
	for (int exponent = 0; exponent <= 20; ++exponent) {
		kappas.push_back(std::ldexp(1.0, exponent));
	}

	AlgorithmState cgs{"CGS"};
	AlgorithmState mgs{"MGS"};
	AlgorithmState cgs2{"CGS2"};
	AlgorithmState mgs2{"MGS2"};

	std::cout << "\n=== " << precision_name << " ===" << std::endl;

	for (double kappa : kappas) {
		DenseMatrix<double> A_high(100, 10);
		randsvd<double>(A_high, kappa, 730, 169);
		const DenseMatrix<T> A = matrix_cast<double, T>(A_high);

		std::cout << "  kappa=" << std::scientific << std::setprecision(3) << kappa << std::endl;

		try_algorithm(A, cgs, kappa,
					  [](const DenseMatrix<T>& input) {
						  return gram_schmidt_mixed<T, T, T, T>(input);
					  });
		try_algorithm(A, mgs, kappa,
					  [](const DenseMatrix<T>& input) {
						  return modified_gram_schmidt_mixed<T, T, T, T>(input);
					  });
		try_algorithm(A, cgs2, kappa,
					  [](const DenseMatrix<T>& input) {
						  return cgs2_mixed<T, T, T, T>(input);
					  });
		try_algorithm(A, mgs2, kappa,
					  [](const DenseMatrix<T>& input) {
						  return mgs2_mixed<T, T, T, T>(input);
					  });

		if (!cgs.active && !mgs.active && !cgs2.active && !mgs2.active) {
			break;
		}
	}

	assert(cgs.saw_success && mgs.saw_success && cgs2.saw_success && mgs2.saw_success);

	std::cout << "  summary:" << std::endl;
	std::cout << "    CGS  max successful kappa = " << std::scientific << std::setprecision(3)
			  << cgs.max_ok_kappa << std::endl;
	std::cout << "    MGS  max successful kappa = " << std::scientific << std::setprecision(3)
			  << mgs.max_ok_kappa << std::endl;
	std::cout << "    CGS2 max successful kappa = " << std::scientific << std::setprecision(3)
			  << cgs2.max_ok_kappa << std::endl;
	std::cout << "    MGS2 max successful kappa = " << std::scientific << std::setprecision(3)
			  << mgs2.max_ok_kappa << std::endl;
}

} // namespace

/**
 * @brief Entry point for the low-precision Gram-Schmidt sweep.
 *
 * Runs the kappa search for FP8 and FP16 when CPFloat support is available.
 */
int main() {
	std::cout << "Low precision Gram-Schmidt sweep" << std::endl;
	std::cout << "Matrix size: 100x10" << std::endl;
	std::cout << "The matrix is generated in double precision, then cast to the target type." << std::endl;

	run_precision_suite<FP8>("FP8");
	run_precision_suite<FP16>("FP16");

	return 0;
}

#else

/**
 * @brief Fallback entry point when CPFloat support is unavailable.
 *
 * The test suite depends on FP8 and FP16, so it only prints a message in
 * builds that do not enable low-precision support.
 */
int main() {
	std::cout << "Low-precision CPFloat support is not enabled in this build." << std::endl;
	return 0;
}

#endif
