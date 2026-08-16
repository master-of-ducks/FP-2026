#/**
 * @file test_baseline.cc
 * @brief Test mixed precision implementation baseline againt single precision implementation.
 */
#include "../../hdnum.hh"
#include "../../src/gram_schmidt_mixed.hh"
#include "../../src/testmatrices.hh"


#include <cassert>
#include <iostream>

using namespace hdnum;

namespace {
// helper to compare matrix entries
template <class T>
bool matrices_exactly_equal(const DenseMatrix<T>& lhs, const DenseMatrix<T>& rhs) {
	if (lhs.rowsize() != rhs.rowsize() || lhs.colsize() != rhs.colsize()) {
		return false;
	}

	for (std::size_t i = 0; i < lhs.rowsize(); ++i) {
		for (std::size_t j = 0; j < lhs.colsize(); ++j) {
			if (lhs[i][j] != rhs[i][j]) {
				return false;
			}
		}
	}

	return true;
}

// helper to compare mixed to single precision implementation for one precision
template <class T>
void check_one_precision(const char* name) {
	DenseMatrix<T> A(100, 10);
    randsvd<T>(A, mixed_cast<T>((double)1), 5, 6);

	const DenseMatrix<T> cgs_standard = gram_schmidt(A);
	const DenseMatrix<T> cgs_mixed = gram_schmidt_mixed<T, T, T, T>(A);
	const DenseMatrix<T> mgs_standard = modified_gram_schmidt(A);
	const DenseMatrix<T> mgs_mixed = modified_gram_schmidt_mixed<T, T, T, T>(A);

	assert(matrices_exactly_equal(cgs_standard, cgs_mixed));
	assert(matrices_exactly_equal(mgs_standard, mgs_mixed));

	std::cout << "Baseline comparison passed for " << name << std::endl;
}

} // namespace

int main() {
#ifdef HDNUM_HAS_CPFLOAT
    check_one_precision<FP8>("FP8");
	check_one_precision<bfloat16>("bfloat16");
    check_one_precision<FP16>("FP16");
#endif
    check_one_precision<FP32>("FP32");
	check_one_precision<FP64>("FP64");
#ifdef HDNUM_HAS_GMP
	check_one_precision<FP128>("FP128");
	check_one_precision<FP256>("FP256");
	check_one_precision<FP512>("FP512");
	check_one_precision<FP1024>("FP1024");
#endif

	return 0;
}
