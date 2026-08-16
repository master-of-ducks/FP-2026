#include "../../hdnum.hh"

#include <iostream>

using namespace hdnum;

int main()
{
    // the functions qr_gram_schmidt_simple and qr_gram_schmidt need small or square matrices (m >= n) with full rank as input
    using T = double;
    DenseMatrix<T> A({{1, 2, 3},
                           {1, 3 ,-5},
                           {2, 4, 19}});

    // the input matrix is overwritten while the decomposition so insure yourself to save it
    DenseMatrix<T> Q(A);

    // the decomposition gives back matrix R so create it using DenseMatrix copy constructor
    DenseMatrix<T> R(qr_gram_schmidt_simple(Q));

    // thats it, now you've created an orthogonal matrix Q and an upper triangular matrix R which applies: A = Q*R
    // this works identically using qr_gram_schmidt
    hdnum::DenseMatrix<T> QR(Q*R);
    std::cout << "QR = " << QR << std::endl;


    // the function qr_gram_schmidt_pivoting works with all types of matrices
    hdnum::DenseMatrix<T> A2({{1, 2, 3, 4},
                                   {-5, 9, 2, 5},
                                   {3, -9, 12, -22}});

    // the input matrix is overwritten again while the decomposition so insure yourself to save it before
    DenseMatrix<T> Q2(A2);

    // the function is a rank revealer, you need a variable to store the rank
    int rank;

    // the function uses column pivoting, you need a permutation vector to store permutations
    Vector<int> p(A2.colsize());

    // the decomposition gives back matrix R so create it using DenseMatrix copy constructor
    // as last argument you can hand over a threhold that defines the functions accuracy, but you don't have to
    DenseMatrix<T> R2(qr_gram_schmidt_pivoting(Q2, p, rank));

    // if the matrix A hasn't full rank -> the dimension of Q and R changes, so you have to create a new Q and R
    // create the new matrix Q
    DenseMatrix<T> Q3(A2.rowsize(), rank);
    for (int i = 0; i < Q3.rowsize(); i++) {
        for (int j = 0; j < Q3.colsize(); j++) {
            Q3(i, j) = Q2(i, j);
        }
    }

    // create the new matrix R
    DenseMatrix<T> R3(rank, A2.colsize());
    for (int i = 0; i < R3.rowsize(); i++) {
        for (int j = 0; j < R3.colsize(); j++) {
            R3(i, j) = R2(i, j);
        }
    }

    // thats it, now you've created an orthogonal matrix Q and an upper triangular matrix R which applies: A = Q*R
    // but you still have to apply the permutations
    DenseMatrix<T> QR2(Q3*R3);
    permute_forward(QR2, p);
    std::cout << "QR2 = " << QR2 << std::endl;
}