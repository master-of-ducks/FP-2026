#/**
 * @file gram_schmidt_mixed.hh
 * @brief Mixed-precision Gram-Schmidt source file.
 *
 * @details Implements six mixed-precision versions of the Gram-Schmidt algorithm and any helpers needed:
 * - a cast between native, low and high precision data types
 * - helpers to read and write matrices in a MatrixMarket file format
 * - various helpers to calculate errors
 * 
 * The first four Gram-Schmidt versions implement CGS/MGS/CGS2/MGS2 using four different precisions
 * for storage data type, inner products, axpy-operation and normalization.
 * 
 * The last two versions implement CGS2/MGS2 using one precision uniformly for the first orthogonalisation
 * and the second for re-orthogonalisation.
 */
#include <cmath>
#include <type_traits>
#include <utility>
#include <stdexcept>

#include "densematrix.hh"
#include "vector.hh"
#include "../hdnum.hh"

#include "lowprec_cpfloat.hh"
#include "highprec_gmp.hh"


namespace hdnum {

// #ifdef HDNUM_HAS_CPFLOAT
// #ifdef HDNUM_HAS_GMP

/**
 * @brief Type-trait that detects whether a type is a CPFloat specialization.
 *
 * @details This trait inherits from `std::true_type` when the inspected type is
 * `CPFloat<m,e>` for some template parameters `m` and `e`, and from
 * `std::false_type` otherwise. It is used in SFINAE-enabled overloads to
 * provide mixed-precision conversions that are specific to the project's
 * low-precision `CPFloat` wrapper.
 *
 * @tparam T The type to inspect.
 */
template <class T>
struct is_cpfloat : std::false_type {};

template <int m, int e>
struct is_cpfloat<CPFloat<m, e>> : std::true_type {};

/**
 * @brief Type-trait that detects whether a type is an `FP<m>` (GMP-backed) type.
 *
 * @details Similar to `is_cpfloat`, this trait resolves to `std::true_type` when the
 * inspected type is an `FP<m>` instantiation and to `std::false_type`
 * otherwise. It enables overload resolution for mixed casts involving GMP
 * high-precision types.
 *
 * @tparam T The type to inspect.
 */
template <class T>
struct is_gmp : std::false_type {};

template <int m>
struct is_gmp<FP<m>> : std::true_type {};

/**
 * @brief Sanitize a floating-point value by mapping NaN or infinite values to zero.
 *
 * @details Mixed-precision conversions sometimes read intermediate values from
 * high-precision representations (for example via `get_d()` or `getNumber()`),
 * which can contain non-finite results when parsing or converting. To avoid
 * propagating NaNs or infinities into lower-precision arithmetic, this helper
 * replaces any non-finite double with `0.0` while leaving finite values
 * unchanged.
 *
 * @param value Input value to sanitize.
 * @return The original value if finite, otherwise `0.0`.
 */
inline double make_finite(double value) {
    return std::isfinite(value) ? value : 0.0;
}

//============================================

/**
 * @brief Convert a CPFloat-backed value to another scalar type.
 *
 * @details This overload is selected when `From` is a `CPFloat<m,e>` specialization.
 * It extracts a double-like representation via `getNumber()` (project-specific
 * API) and sanitizes it with `make_finite()` before constructing the target
 * type `To`.
 *
 * @tparam To Destination scalar type.
 * @tparam From Source scalar type (CPFloat specialization).
 * @param value Value to convert.
 * @return Converted value of type `To`.
 */
template <class To, class From,
          typename std::enable_if<is_cpfloat<From>::value,
                                  int>::type Enable = 0>
inline To mixed_cast(const From& value) {
    return To(make_finite(value.getNumber()));
}

/**
 * @brief Convert a GMP `FP<m>` value to another GMP `FP<n>` value.
 *
 * @details When both source and destination are GMP-backed types this overload performs
 * a direct construction from `From` to `To` allowing GMP's internal
 * conversion routines to manage precision and rounding.
 *
 * @tparam To Destination GMP-backed type.
 * @tparam From Source GMP-backed type.
 * @param value Value to convert.
 * @return Converted value of type `To`.
 */
template <class To, class From,
          typename std::enable_if<is_gmp<From>::value && is_gmp<To>::value,
                                  int>::type Enable = 0>
inline To mixed_cast(const From& value) {
    return To(value);
}

/**
 * @brief Convert a GMP `FP<m>` value into a non-GMP scalar type.
 *
 * @details This overload extracts a double approximation from the GMP value using
 * `getNumber().get_d()` and sanitizes it with `make_finite()` before
 * constructing the destination type. Use this when downcasting from GMP to
 * built-in or project-local types.
 *
 * @tparam To Destination scalar type (not GMP-backed).
 * @tparam From Source GMP-backed type.
 * @param value Value to convert.
 * @return Converted value of type `To`.
 */
template <class To, class From,
          typename std::enable_if<is_gmp<From>::value && !is_gmp<To>::value,
                                  int>::type Enable = 0>
inline To mixed_cast(const From& value) {
    return To(make_finite(value.getNumber().get_d()));
}

// Cast from float/double
/**
 * @brief Convert native floating-point values (`float`/`double`) to `To`.
 *
 * @details This overload handles the common case of converting built-in floating-point
 * types into the project's scalar types. It first promotes to `double`, runs
 * `make_finite()` to avoid NaN/Inf propagation, and then constructs `To`.
 *
 * @tparam To Destination scalar type.
 * @tparam From Source scalar type (`float` or `double`).
 * @param value Value to convert.
 * @return Converted value of type `To`.
 */
template <class To, class From,
          typename std::enable_if<std::is_same<From, float>::value ||
                                  std::is_same<From, double>::value,
                                  int>::type Enable = 0>
inline To mixed_cast(const From& value) {
    return To(make_finite(static_cast<double>(value)));
}

//============================================

/**
 * @brief Compute the Frobenius norm of a dense matrix.
 *
 * @details The Frobenius norm is defined as sqrt(sum_{i,j} |A_{i,j}|^2). The
 * implementation accumulates the sum in the template scalar type `T1` and
 * calls `sqrt` on the accumulated value. The cost is O(m*n) for an m-by-n
 * matrix.
 *
 * @tparam T1 Scalar type stored in the matrix and returned as the norm type.
 * @param A Matrix whose Frobenius norm is to be computed.
 * @return Frobenius norm of the matrix as a `T1` value.
 */
template <typename T1>
static T1 frobenius_norm(const DenseMatrix<T1>& A) {
    // using std::sqrt;
    T1 sum = T1(0.0);
    for (std::size_t i = 0; i < A.rowsize(); ++i) {
        for (std::size_t j = 0; j < A.colsize(); ++j) {
            sum += A[i][j] * A[i][j];
        }
    }
    return sqrt(sum);
}

/**
 * @brief Create an n-by-m identity-like matrix.
 *
 * @details The function constructs a DenseMatrix filled with zeros and sets the
 * diagonal entries to `1.0` up to the smaller of `n` and `m`. This is useful
 * for constructing square identity matrices as well as rectangular identity
 * blocks used in projection checks.
 *
 * @tparam T Scalar type of the returned matrix entries.
 * @param n Number of rows of the returned matrix.
 * @param m Number of columns of the returned matrix.
 * @return DenseMatrix<T> with ones on the main diagonal and zeros elsewhere.
 */
template <typename T>
static DenseMatrix<T> make_identity(std::size_t n, std::size_t m) {
    DenseMatrix<T> I(n, m,T(0.0));
    for (std::size_t i = 0; i < std::min(n,m); ++i) {
        I[i][i] = T(1.0);
    }
    return I;
}

/**
 * @brief Convert a dense matrix from one scalar type to another.
 *
 * @details Each entry in the input matrix `A_low` is converted to the destination
 * scalar type `T2` using the `mixed_cast<T2>` helper. This ensures
 * project-specific rounding and sanitization rules are applied during the
 * conversion (for example mapping non-finite values to zero).
 *
 * @tparam T1 Input scalar type.
 * @tparam T2 Output scalar type.
 * @param A_low Matrix in the input scalar type.
 * @return A newly allocated DenseMatrix<T2> with entries converted from `A_low`.
 */
template <typename T1, typename T2>
DenseMatrix<T2> matrix_cast(const DenseMatrix<T1>& A_low) {
    DenseMatrix<T2> A(A_low.rowsize(), A_low.colsize(), 0);
    for (int i = 0; i < A_low.rowsize(); i++) {
        for (int j = 0; j < A_low.colsize(); j++) {
            A [i][j] = mixed_cast<T2>(A_low[i][j]);
        }
    }
    return A;
}

/**
 * @brief Write a dense matrix in a simple Matrix Market coordinate-like format.
 *
 * @details The function writes the matrix dimensions and the number of non-zero
 * entries followed by one entry per line with 1-based row and column
 * indices and the value. It uses a very high `std::setprecision(300)` to
 * preserve as much precision as possible for high-precision scalar types.
 *
 * @tparam REAL Scalar type used for the matrix entries.
 * @param A Matrix to serialize.
 * @param filename Path to write the output file to. If the file cannot be
 * opened an error is logged via `HDNUM_ERROR` and the function returns.
 */
template <typename REAL>
void writeMatrixMarketHighPrec(const DenseMatrix<REAL>& A, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        HDNUM_ERROR("Error opening file: " + filename);
        return;
    }

    const std::size_t rows = A.rowsize();
    const std::size_t cols = A.colsize();

    // count non-zero entries (MatrixMarket coordinate format)
    std::size_t nnz = 0;
    for (std::size_t i = 0; i < rows; ++i) {
        for (std::size_t j = 0; j < cols; ++j) {
            if (A[i][j] != REAL(0)) ++nnz;
        }
    }

    // Header: rows cols nnz
    file << rows << " " << cols << " " << nnz << "\n";

    // Write entries as 1-based indices
    file << std::setprecision(300);
    for (std::size_t i = 0; i < rows; ++i) {
        for (std::size_t j = 0; j < cols; ++j) {
            const REAL val = A[i][j];
            if (val != REAL(0)) {
                file << (i + 1) << " " << (j + 1) << " " << val << "\n";
            }
        }
    }
    file.close();
}

#/**
 * @brief Parse a single numeric token from a Matrix Market-style file.
 *
 * @details When the codebase is built with GMP support (`HDNUM_HAS_GMP`), the token
 * is parsed into an `mpf_class` with an extended precision mantissa and then
 * converted to the requested `REAL` type. Otherwise the token is parsed via
 * a `std::istringstream` into `REAL`.
 *
 * @tparam REAL Destination scalar type for the parsed value.
 * @param token The textual numeric token read from the file.
 * @return The parsed value as `REAL`.
 * @throws std::invalid_argument if the token cannot be parsed as a number.
 */
template <typename REAL>
inline REAL parseMatrixMarketValue(const std::string& token) {
#ifdef HDNUM_HAS_GMP
    mpf_class parsed_value(0, 4096);
    if (parsed_value.set_str(token.c_str(), 10) != 0) {
        throw std::invalid_argument("Invalid MatrixMarket numeric value: " + token);
    }
    return REAL(parsed_value);
#else
    std::istringstream value_stream(token);
    REAL value{};
    value_stream >> value;
    if (!value_stream) {
        throw std::invalid_argument("Invalid MatrixMarket numeric value: " + token);
    }
    return value;
#endif
}

/**
 * @brief Read a simple Matrix Market-like coordinate file into `A`.
 *
 * @details The reader expects zero or more comment lines starting with '%', followed
 * by a first non-comment line containing the number of rows and columns. The
 * following lines should contain triples `i j value` with 1-based indices.
 * Values are parsed with `parseMatrixMarketValue<REAL>` so high-precision
 * parsing is available when compiled with GMP.
 *
 * The function checks that the file dimensions match the already-allocated
 * matrix `A` and logs an error via `HDNUM_ERROR` if they do not match.
 *
 * @tparam REAL Scalar type of matrix entries to produce.
 * @param A Output matrix that will be filled with the file entries. Its
 * dimensions must match those declared in the file.
 * @param filename Path to the input Matrix Market file.
 */
template <typename REAL, typename std::enable_if<is_gmp<REAL>::value,
                                  int>::type Enable = 0>
inline void readMatrixFromFileMatrixMarketHighPrec(DenseMatrix<REAL>& A,
                                           const std::string& filename) {
    std::string buffer;
    std::ifstream fin(filename.c_str());
    std::size_t i = 0;
    std::size_t j = 0;
    if (fin.is_open()) {
        // ignore all comments from the file (starting with %)
        while (fin.peek() == '%') fin.ignore(2048, '\n');

        std::getline(fin, buffer);
        std::istringstream first_line(buffer);
        first_line >> i >> j;
        DenseMatrix<REAL> A_temp(i, j);

        if (A.colsize() != A_temp.colsize() || A.rowsize() != A_temp.rowsize()) {
            HDNUM_ERROR("MatrixMarket file dimesions do not match A");
        }

        while (std::getline(fin, buffer)) {
            std::istringstream iss(buffer);

            std::string valstr;
            if (!(iss >> i >> j >> valstr))
                continue;
            const REAL a_ij = parseMatrixMarketValue<REAL>(valstr);
            A_temp(i - 1, j - 1) = a_ij;
        }
        A = A_temp;
        fin.close();
    } else {
        HDNUM_ERROR("Could not open file! \"" + filename + "\"");
    }
}

/**
 * @brief Read a simple Matrix Market-like coordinate file into `A` (overload for non GMP types).
 *
 * @details The reader expects zero or more comment lines starting with '%', followed
 * by a first non-comment line containing the number of rows and columns. The
 * following lines should contain triples `i j value` with 1-based indices.
 * Values are parsed with `parseMatrixMarketValue<REAL>` so high-precision
 * parsing is available when compiled with GMP.
 *
 * The function checks that the file dimensions match the already-allocated
 * matrix `A` and logs an error via `HDNUM_ERROR` if they do not match.
 *
 * @tparam REAL Scalar type of matrix entries to produce.
 * @param A Output matrix that will be filled with the file entries. Its
 * dimensions must match those declared in the file.
 * @param filename Path to the input Matrix Market file.
 */
template <typename REAL,
          typename std::enable_if<std::is_same<REAL, float>::value ||
                                  std::is_same<REAL, double>::value ||
                                  is_cpfloat<REAL>::value,
                                  int>::type Enable = 0>
inline void readMatrixFromFileMatrixMarketHighPrec(DenseMatrix<REAL>& A,
                                           const std::string& filename) {
    std::string buffer;
    std::ifstream fin(filename.c_str());
    std::size_t i = 0;
    std::size_t j = 0;
    if (fin.is_open()) {
        // ignore all comments from the file (starting with %)
        while (fin.peek() == '%') fin.ignore(2048, '\n');

        std::getline(fin, buffer);
        std::istringstream first_line(buffer);
        first_line >> i >> j;
        DenseMatrix<REAL> A_temp(i, j);

        if (A.colsize() != A_temp.colsize() || A.rowsize() != A_temp.rowsize()) {
            HDNUM_ERROR("MatrixMarket file dimesions do not match A");
        }

        while (std::getline(fin, buffer)) {
            std::istringstream iss(buffer);

            std::string valstr;
            if (!(iss >> i >> j >> valstr))
                continue;
            const REAL a_ij = mixed_cast<REAL>(std::stod(valstr));
            A_temp(i - 1, j - 1) = a_ij;
        }
        A = A_temp;
        fin.close();
    } else {
        HDNUM_ERROR("Could not open file! \"" + filename + "\"");
    }
}

#pragma region GS_implementation

/*! \brief Performs Gram-Schmidt orthogonalization with mixed precision on a matrix.
 *  \param A The input matrix.
 *  \details Template parameters correspond to the types of the input matrix, and the precision parameters used for rounding.
 *  T1: Type of the input matrix A.
 *  T2: Type of inner products
 *  T3: Type for projections (axpy-operations)
 *  T4: Type for normalizations
 *  \return The orthogonalized matrix.
 */
template <class T1, class T2, class T3, class T4>
DenseMatrix<T1> cgs_mixed(const DenseMatrix<T1>& A) {
    using std::sqrt;
    DenseMatrix<T1> Q(A);

    // for all columns except the first
    for (int k = 1; k < Q.colsize(); k++) {
        // orthogonalize column k against all previous
        for (int j = 0; j < k; j++) {
            // compute factor
            T2 sum_nom(0.0);
            T2 sum_denom(0.0);
            for (int i = 0; i < Q.rowsize(); i++) {
                const T2 a_k = mixed_cast<T2>(A[i][k]);
                const T2 q_ij = mixed_cast<T2>(Q[i][j]);
                sum_nom += a_k * q_ij;
                sum_denom += q_ij * q_ij;
            }
            // modify
            if (sum_denom == T2(0.0)) {
                throw std::invalid_argument("CGS: Encountered zero denominator in modification");
                }
            const T3 alpha = mixed_cast<T3>(sum_nom) / mixed_cast<T3>(sum_denom);
            for (int i = 0; i < Q.rowsize(); i++) {
                const T3 q_ij = mixed_cast<T3>(Q[i][j]);
                T3 q_ik = mixed_cast<T3>(Q[i][k]);
                q_ik -= (alpha * q_ij);
                Q[i][k] = mixed_cast<T1>(q_ik);
            }
        }
    }
    for (int j = 0; j < Q.colsize(); j++) {
        // compute norm of column j
        T4 sum(0.0);
        for (int i = 0; i < Q.rowsize(); i++) {
            const T4 q_ij = mixed_cast<T4>(Q[i][j]);
            sum += q_ij * q_ij;
        }
        if (sum == T4(0.0)) {
            throw std::invalid_argument("CGS: Encountered zero denominator in normalization");
        }
        T4 norm = sqrt(mixed_cast<T4>(sum));
        // scale
        for (int i = 0; i < Q.rowsize(); i++) {
            const T4 q_ij = mixed_cast<T4>(Q[i][j]);
            Q[i][j] = mixed_cast<T1>(q_ij / norm);
        }
    }
    return Q;
}

/*! \brief Performs Gram-Schmidt with reorthogonalization with mixed precision on a matrix.
 *  \param A The input matrix.
 *  \details Template parameters correspond to the types of the input matrix, and the precision parameters used for rounding.
 *  T1: Type of the input matrix A.
 *  T2: Type of inner products
 *  T3: Type for projections (axpy-operations)
 *  T4: Type for normalizations
 *  \return The orthogonalized matrix.
 */
template <class T1, class T2, class T3, class T4>
DenseMatrix<T1> cgs2_mixed(const DenseMatrix<T1>& A) {
    using std::sqrt;
    DenseMatrix<T1> Q(A);
    DenseMatrix<T1> A2(A);
    // perform projection twice
    for (int r = 0; r < 2; r++) {
        // for all columns except the first
        for (int k = 1; k < Q.colsize(); k++) {
            // orthogonalize column k against all previous
            for (int j = 0; j < k; j++) {
                // compute factor
                T2 sum_nom(0.0);
                T2 sum_denom(0.0);
                for (int i = 0; i < Q.rowsize(); i++) {
                    const T2 a_k = mixed_cast<T2>(A2[i][k]);
                    const T2 q_ij = mixed_cast<T2>(Q[i][j]);
                    sum_nom += a_k * q_ij;
                    sum_denom += q_ij * q_ij;
                }
                // modify
                if (sum_denom == T2(0.0)) {
                    throw std::invalid_argument("CGS2: Encountered zero denominator in modification");
                    }
                const T3 alpha = mixed_cast<T3>(sum_nom) / mixed_cast<T3>(sum_denom);
                for (int i = 0; i < Q.rowsize(); i++) {
                    const T3 q_ij = mixed_cast<T3>(Q[i][j]);
                    T3 q_ik = mixed_cast<T3>(Q[i][k]);
                    q_ik -= (alpha * q_ij);
                    Q[i][k] = mixed_cast<T1>(q_ik);
                }
            }
        }
        A2 = Q;
    }
    for (int j = 0; j < Q.colsize(); j++) {
        // compute norm of column j
        T4 sum(0.0);
        for (int i = 0; i < Q.rowsize(); i++) {
            const T4 q_ij = mixed_cast<T4>(Q[i][j]);
            sum += q_ij * q_ij;
        }
        if (sum == T4(0.0)) {
            throw std::invalid_argument("CGS2: Encountered zero denominator in normalization");
        }
        T4 norm = sqrt(mixed_cast<T4>(sum));
        // scale
        for (int i = 0; i < Q.rowsize(); i++) {
            const T4 q_ij = mixed_cast<T4>(Q[i][j]);
            Q[i][j] = mixed_cast<T1>(q_ij / norm);
        }
    }
    return Q;
}

/*! \brief Performs classical Gram-Schmidt orthogonalization twice, first in lower and then in higher precision 
 *  \param A The input matrix.
 *  \details Template parameters correspond to the types of the input matrix, and the precision parameters used for rounding.
 *  T1: Type of the input matrix A and precision of first CGS run
 *  T2: Type of the output matrix and precision of second CGS run
 *  \return The orthogonalized matrix.
 */
template <class T1, class T2>
DenseMatrix<T2> cgs2_2_mixed(const DenseMatrix<T1>& A) {
    DenseMatrix<T1> Q(A);
    Q = cgs_mixed<T1,T1,T1,T1>(Q);
    DenseMatrix<T2> Q2 = matrix_cast<T1,T2>(Q);
    Q2 = cgs_mixed<T2,T2,T2,T2>(Q2);

    return Q2;
}

/*! \brief Performs modified Gram-Schmidt orthogonalization with mixed precision on a matrix.
 *  \param A The input matrix.
 *  \details Template parameters correspond to the types of the input matrix, and the precision parameters used for rounding.
 *  T1: Type of the input matrix A.
 *  T2: Type of inner products
 *  T3: Type for projections (axpy-operations)
 *  T4: Type for normalizations
 *  \return The orthogonalized matrix.
 */
template <class T1, class T2, class T3, class T4>
DenseMatrix<T1> mgs_mixed(const DenseMatrix<T1>& A) {
    using std::sqrt;
    DenseMatrix<T1> Q(A);
    for (int k = 0; k < Q.colsize(); k++) {
        // modify all later columns with column k
        for (int j = k + 1; j < Q.colsize(); j++) {
            // compute factor
            T2 sum_nom(0.0);
            T2 sum_denom(0.0);
            for (int i = 0; i < Q.rowsize(); i++) {
                const T2 q_ik = mixed_cast<T2>(Q[i][k]);
                const T2 q_ij = mixed_cast<T2>(Q[i][j]);
                sum_nom += q_ij * q_ik;
                sum_denom += q_ik * q_ik;
            }
            // modify
            if (sum_denom == T2(0.0)) {
                throw std::invalid_argument("MGS: Encountered zero denominator in modification");
            }
            T3 alpha =  mixed_cast<T3>(sum_nom) / mixed_cast<T3>(sum_denom);
            for (int i = 0; i < Q.rowsize(); i++) {
                const T3 q_ik = mixed_cast<T3>(Q[i][k]);
                T3 q_ij = mixed_cast<T3>(Q[i][j]);
                q_ij -= (alpha * q_ik);
                Q[i][j] = mixed_cast<T1>(q_ij);

            }
        }
    }
    for (int j = 0; j < Q.colsize(); j++) {
        // compute norm of column j
        T4 sum(0.0);
        for (int i = 0; i < Q.rowsize(); i++) {
            const T4 q_ij = mixed_cast<T4>(Q[i][j]);
            sum += q_ij * q_ij;
        }
        if (sum == T4(0.0)) {
            throw std::invalid_argument("MGS: Encountered zero denominator in normalization");
        }
        T4 norm = sqrt(mixed_cast<T4>(sum));
        // scale
        for (int i = 0; i < Q.rowsize(); i++) {
            const T4 q_ij = mixed_cast<T4>(Q[i][j]);
            Q[i][j] = mixed_cast<T1>(q_ij / norm);
        }
    }
    return Q;
}

/*! \brief Performs modified Gram-Schmidt with reorthogonalization with mixed precision on a matrix.
 *  \param A The input matrix.
 *  \details Template parameters correspond to the types of the input matrix, and the precision parameters used for rounding.
 *  T1: Type of the input matrix A.
 *  T2: Type of inner products
 *  T3: Type for projections (axpy-operations)
 *  T4: Type for normalizations
 *  \return The orthogonalized matrix.
 */
template <class T1, class T2, class T3, class T4>
DenseMatrix<T1> mgs2_mixed(const DenseMatrix<T1>& A) {
    using std::sqrt;
    DenseMatrix<T1> Q(A);
    // perform projection twice
    for (int r = 0; r < 2; r++) {
        for (int k = 0; k < Q.colsize(); k++) {
            // modify all later columns with column k
            for (int j = k + 1; j < Q.colsize(); j++) {
                // compute factor
                T2 sum_nom(0.0);
                T2 sum_denom(0.0);
                for (int i = 0; i < Q.rowsize(); i++) {
                    const T2 q_ik = mixed_cast<T2>(Q[i][k]);
                    const T2 q_ij = mixed_cast<T2>(Q[i][j]);
                    sum_nom += q_ij * q_ik;
                    sum_denom += q_ik * q_ik;
                }
                // modify
                if (sum_denom == T2(0.0)) {
                    throw std::invalid_argument("MGS2: Encountered zero denominator in modification");
                }
                T3 alpha =  mixed_cast<T3>(sum_nom) / mixed_cast<T3>(sum_denom);
                for (int i = 0; i < Q.rowsize(); i++) {
                    const T3 q_ik = mixed_cast<T3>(Q[i][k]);
                    T3 q_ij = mixed_cast<T3>(Q[i][j]);
                    q_ij -= (alpha * q_ik);
                    Q[i][j] = mixed_cast<T1>(q_ij);

                }
            }
        }
    }
    for (int j = 0; j < Q.colsize(); j++) {
        // compute norm of column j
        T4 sum(0.0);
        for (int i = 0; i < Q.rowsize(); i++) {
            const T4 q_ij = mixed_cast<T4>(Q[i][j]);
            sum += q_ij * q_ij;
        }
        if (sum == T4(0.0)) {
            throw std::invalid_argument("MGS2: Encountered zero denominator in normalization");
        }
        T4 norm = sqrt(mixed_cast<T4>(sum));
        // scale
        for (int i = 0; i < Q.rowsize(); i++) {
            const T4 q_ij = mixed_cast<T4>(Q[i][j]);
            Q[i][j] = mixed_cast<T1>(q_ij / norm);
        }
    }
    return Q;
}

/*! \brief Performs modified Gram-Schmidt orthogonalization twice, first in lower and then in higher precision 
 *  \param A The input matrix.
 *  \details Template parameters correspond to the types of the input matrix, and the precision parameters used for rounding.
 *  T1: Type of the input matrix A and precision of first CGS run
 *  T2: Type of the output matrix and precision of second CGS run
 *  \return The orthogonalized matrix.
 */
template <class T1, class T2>
DenseMatrix<T2> mgs2_2_mixed(const DenseMatrix<T1>& A) {
    DenseMatrix<T1> Q(A);
    Q = mgs_mixed<T1,T1,T1,T1>(Q);
    DenseMatrix<T2> Q2 = matrix_cast<T1,T2>(Q);
    Q2 = mgs_mixed<T2,T2,T2,T2>(Q2);

    return Q2;
}

#pragma endregion GS_implementation

#pragma region error_calculation
/**
 * @brief Container for orthogonality and representation error metrics.
 *
 * @details All fields use `FP1024` (very high precision) so that errors computed from
 * lower-precision experiments can be compared without loss of resolution.
 *
 * Fields named `ortho_err_*` store the Frobenius norm of `Q^T Q - I` for the
 * corresponding algorithm variant, while `rep_err_*` stores the relative
 * representation error `||A - Q(Q^T A)||_F / ||A||_F`.
 */
struct Result {
    FP1024 ortho_err_cgs;
    FP1024 ortho_err_mgs;
    FP1024 ortho_err_cgs2;
    FP1024 ortho_err_mgs2;
    FP1024 rep_err_cgs;
    FP1024 rep_err_mgs;
    FP1024 rep_err_cgs2;
    FP1024 rep_err_mgs2;
};


/**
 * @brief Compute orthogonality and representation errors in FP1024 precision.
 *
 * @details The function converts `A_low` and `Q_low` to `FP1024` and computes two
 * metrics used throughout the experiments:
 * - Orthogonality error: Frobenius norm of `Q^T Q - I`.
 * - Representation error: `||A - Q(Q^T A)||_F / ||A||_F` (relative error).
 *
 * @tparam T Scalar type of the input matrices provided by the Gram-Schmidt
 *           routines. Both `A_low` and `Q_low` are converted to `FP1024` for
 *           the error calculations.
 * @param A_low Original matrix before orthogonalization.
 * @param Q_low Orthogonalized matrix produced by a Gram-Schmidt routine.
 * @return A `std::pair<FP1024,FP1024>` where `.first` is the orthogonality
 *         error and `.second` is the relative representation error.
 */
template <typename T>
std::pair<FP1024, FP1024> calculate_errors(const DenseMatrix<T>& A_low, const DenseMatrix<T>& Q_low) {
    DenseMatrix<FP1024> A = matrix_cast<T, FP1024>(A_low);
    DenseMatrix<FP1024> Q = matrix_cast<T, FP1024>(Q_low);
    const DenseMatrix<FP1024> I = make_identity<FP1024>(A_low.colsize(), A_low.colsize());
    const DenseMatrix<FP1024> QtQ = Q.transpose() * Q;
    const DenseMatrix<FP1024> Aproj = Q * (Q.transpose() * A);
    if (QtQ.rowsize() != I.rowsize()) std::cout << "QtQ.rowsize() != I.rowsize()" << std::endl;
    FP1024 ortho_err = frobenius_norm<FP1024>(QtQ - I);
    if (A.rowsize() != Aproj.rowsize()) std::cout << "A.rowsize() != Aproj.rowsize()" << std::endl;
    FP1024 rep_err = frobenius_norm<FP1024>(A - Aproj) / frobenius_norm<FP1024>(A);
    return std::make_pair(ortho_err, rep_err);
}

/**
 * @brief Execute a set of mixed-precision Gram-Schmidt experiments and collect errors.
 *
 * @details For the provided precision tuple (`T1`,`T2`,`T3`,`T4`), this function
 * attempts to run the four orthogonalization routines (CGS, MGS, CGS2, MGS2).
 * Each routine is invoked inside a `try`/`catch` block: if an algorithm
 * raises `std::invalid_argument` (e.g. due to a zero denominator), the
 * exception is logged and the corresponding fields in the returned `Result`
 * are left uninitialized for that run.
 * Errors are reported in `FP1024`.
 *
 * @tparam T1 Type of the input matrix `A` and algorithm I/O.
 * @tparam T2 Type used for inner products during the algorithms.
 * @tparam T3 Type used for projection operations (axpy-like updates).
 * @tparam T4 Type used for norm computations and scaling.
 * @param A The input matrix to orthogonalize (may be modified by some variants).
 * @param combo_label Human-readable label for the precision combination used
 *                    (used only in logged error messages).
 * @param kappa String representation of the input matrix condition number
 *              (used only in logged error messages).
 * @return A `Result` containing orthogonality and representation errors
 *         computed in `FP1024` precision for each algorithm that completed
 *         successfully.
 */
template <typename T1, typename T2, typename T3, typename T4>
Result run_gram_schmidt_mixed(DenseMatrix<T1>& A, std::string combo_label, std::string kappa)  {
    Result result;
    try
    {
        // try to run mixed precision classical Gram-Schmidt
        const DenseMatrix<T1> Q = cgs_mixed<T1, T2, T3, T4>(A);
        // compute orthogonality and representation errors
        std::pair<FP1024, FP1024> errors = calculate_errors<T1>(A, Q);
        result.ortho_err_cgs = errors.first;
        result.rep_err_cgs = errors.second;
    }
    catch(const std::invalid_argument& e)
    {
        std::cerr << combo_label << ": (kappa= " << kappa << ") " << e.what() << '\n';
    }

    try
    {
        // try to run mixed precision modified Gram-Schmidt
        const DenseMatrix<T1> Q = mgs_mixed<T1, T2, T3, T4>(A);
        // compute orthogonality and representation errors
        std::pair<FP1024, FP1024> errors = calculate_errors<T1>(A, Q);
        result.ortho_err_mgs = errors.first;
        result.rep_err_mgs = errors.second;
    }
    catch(const std::invalid_argument& e)
    {
        std::cerr << combo_label << ": (kappa= " << kappa << ") " << e.what() << '\n';
    }

    try
    {
        // try to run mixed precision CGS2
        const DenseMatrix<T1> Q = cgs2_mixed<T1, T2, T3, T4>(A);
        // compute orthogonality and representation errors
        std::pair<FP1024, FP1024> errors = calculate_errors<T1>(A, Q);
        result.ortho_err_cgs2 = errors.first;
        result.rep_err_cgs2 = errors.second;
    }
    catch(const std::invalid_argument& e)
    {
        std::cerr << combo_label << ": (kappa= " << kappa << ") " << e.what() << '\n';
    }
    
    try
    {
        // try to run mixed precision MGS2
        const DenseMatrix<T1> Q = mgs2_mixed<T1, T2, T3, T4>(A);
        // compute orthogonality and representation errors
        std::pair<FP1024, FP1024> errors = calculate_errors<T1>(A, Q);
        result.ortho_err_mgs2 = errors.first;
        result.rep_err_mgs2 = errors.second;
    }
    catch(const std::invalid_argument& e)
    {
        std::cerr << combo_label << ": (kappa= " << kappa << ") " << e.what() << '\n';
    }

    return result;
}

/**
 * @brief Run two-stage CGS2/MGS2 experiments: low-precision then high-precision.
 *
 * @details For the provided precision tuple (`T1`,`T2`), this function
 * attempts to run the four orthogonalization routines (CGS, MGS, CGS2, MGS2).
 * Here CGS and MGS are run as single-precision variant using 'T2'.
 * CGS2 and MGS2 are performed as a single-precision 'T1' run of CGS/MGS followed by
 * a second iteration using precision 'T2'.
 * Each routine is invoked inside a `try`/`catch` block: if an algorithm
 * raises `std::invalid_argument` (e.g. due to a zero denominator), the
 * exception is logged and the corresponding fields in the returned `Result`
 * are left uninitialized for that run.
 * Errors are reported in `FP1024`.
 *
 * @tparam T1 Type used for the first (lower) precision stage.
 * @tparam T2 Type used for the second (higher) precision stage and the
 *           matrix passed into this routine.
 * @param A Input matrix stored in the `T2` precision (will not be modified).
 * @param combo_label Human-readable label for logging and error messages.
 * @param kappa String describing the condition number (used only for logs).
 * @return A `Result` with orthogonality and representation errors for each
 *         algorithm that completed successfully.
 */
template <typename T1, typename T2>
Result run_gram_schmidt_mixed2(DenseMatrix<T2>& A, std::string combo_label, std::string kappa)  {
    Result result;
    DenseMatrix<T1> A1 = matrix_cast<T2,T1>(A);
    try
    {
        // try to run mixed precision classical Gram-Schmidt
        const DenseMatrix<T2> Q = cgs_mixed<T2, T2, T2, T2>(A);
        // compute orthogonality and representation errors
        std::pair<FP1024, FP1024> errors = calculate_errors<T2>(A, Q);
        result.ortho_err_cgs = errors.first;
        result.rep_err_cgs = errors.second;
    }
    catch(const std::invalid_argument& e)
    {
        std::cerr << combo_label << ": (kappa= " << kappa << ") " << e.what() << '\n';
    }

    try
    {
        // try to run mixed precision modified Gram-Schmidt
        const DenseMatrix<T2> Q = mgs_mixed<T2, T2, T2, T2>(A);
        // compute orthogonality and representation errors
        std::pair<FP1024, FP1024> errors = calculate_errors<T2>(A, Q);
        result.ortho_err_mgs = errors.first;
        result.rep_err_mgs = errors.second;
    }
    catch(const std::invalid_argument& e)
    {
        std::cerr << combo_label << ": (kappa= " << kappa << ") " << e.what() << '\n';
    }

    try
    {
        // try to run mixed precision CGS2
        const DenseMatrix<T2> Q = cgs2_2_mixed<T1, T2>(A1);
        // compute orthogonality and representation errors
        std::pair<FP1024, FP1024> errors = calculate_errors<T2>(A, Q);
        result.ortho_err_cgs2 = errors.first;
        result.rep_err_cgs2 = errors.second;
    }
    catch(const std::invalid_argument& e)
    {
        std::cerr << combo_label << ": (kappa= " << kappa << ") " << e.what() << '\n';
    }
    
    try
    {
        // try to run mixed precision MGS2
        const DenseMatrix<T2> Q = mgs2_2_mixed<T1, T2>(A1);
        // compute orthogonality and representation errors
        std::pair<FP1024, FP1024> errors = calculate_errors<T2>(A, Q);
        result.ortho_err_mgs2 = errors.first;
        result.rep_err_mgs2 = errors.second;
    }
    catch(const std::invalid_argument& e)
    {
        std::cerr << combo_label << ": (kappa= " << kappa << ") " << e.what() << '\n';
    }

    return result;
}

#pragma endregion error_calculation
// #endif
// #endif

}  // namespace hdnum