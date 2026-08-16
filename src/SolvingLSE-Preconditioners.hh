// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
/*
 * File:   SolvingLSE-Preconditioners.hh
 * Author: Thomas Gantz <thomas.gantz@stud.uni-heidelberg.de>, ...
 *
 * Created on August 03, 2024
 */

#include "../hdnum.hh"
#include <memory> // For std::shared_ptr
#include <vector> // For std::vector

#ifndef SOLVINGLSE_PRECONDITIONERS_HH
#define SOLVINGLSE_PRECONDITIONERS_HH

namespace hdnum {

    // Richardson preconditioner template class for any type
    template<class T>
    class Richardson : public LinearOperator<T> {
    private:
        T o;  // Omega value for the preconditioner
    
    public:
        // Constructor that takes an omega value and computes its reciprocal
        Richardson(T omega) { o = 1 / omega; }

        // Override the apply method to implement Richardson preconditioning
        void apply(const Vector<T>& d, Vector<T>& v) const override {
            // Perform the preconditioning step: solve Mv = d where M = omega^{-1} * I_n
            for (std::size_t i = 0; i < v.size(); ++i) {
                v[i] = o * d[i];
            } 
        }
    };

    // Jacobi preconditioner template class for any matrix type
    template<class Matrix>
    class Jacobi : public LinearOperator<typename Matrix::value_type> {
    public:
        using T = typename Matrix::value_type;  
    
    private:
        Vector<T> m;  // Vector to store the inverse of the diagonal elements of the matrix
    
    public:
        // Constructor that takes a shared pointer to a matrix
        Jacobi(std::shared_ptr<Matrix> ptr_A) : m(ptr_A->rowsize()) {
            // Compute the inverse of the diagonal elements of the matrix and store in m
            for (std::size_t i = 0; i < ptr_A->rowsize(); i++) {
                m[i] = 1 / (*ptr_A)(i, i);
            }
        }

        // Override the apply method to implement the Jacobi preconditioning
        void apply(const Vector<T>& d, Vector<T>& v) const override {
            // Perform the preconditioning step: solve Mv = d where M = diag(A)
            for (std::size_t i = 0; i < v.size(); ++i) {
                v[i] = m[i] * d[i];
            } 
        }
    };

    template<class Matrix>
    class GaussSeidel : public LinearOperator<typename Matrix::value_type> {
        public:
            using T = typename Matrix::value_type;
            //Attributes
            std::shared_ptr<Matrix> A;

            //Constructor
            GaussSeidel(std::shared_ptr<Matrix> A_) : A(A_) {}

            void apply(const Vector<T>& d, Vector<T>& v) const override {
                
                for (auto row_it = A->begin(); row_it != A->end(); ++row_it) {
                    T sum = 0;
                    T diag = 0;
                    //Iterate over Columns
                    for (auto col_it = row_it.begin(); col_it != row_it.end(); ++col_it)
                    {
                        //Iterate over Rows
                        if (col_it.index() < row_it.index())
                        {
                            sum += col_it.value() * v[col_it.index()];
                            
                        } else if (col_it.index() == row_it.index()){
                            diag = col_it.value();
                        }
                    }
                    v[row_it.index()] = (d[row_it.index()] - sum)/diag;

                }
            }
    };

    template<class Matrix>
    class Kaczmarz : public LinearOperator<typename Matrix::value_type> {
        private:
            std::shared_ptr<Matrix> A;
        
        public:
            using T = typename Matrix::value_type;

            // Constructor
            Kaczmarz(std::shared_ptr<Matrix> A_): A(A_) {}

            void apply(const Vector<T>& d, Vector<T>& v) const override {
            
                // Iterate over each row of A
                for(auto row_it = A->begin(); row_it != A->end(); ++row_it){
                    // Dot product of the current row with v
                    T rowDotProduct = 0;
                    // Squared 2-norm of the current row
                    T row2NormSquared = 0;

                    // Iterate over each non-zero element in the current row
                    for(auto col_it = row_it.begin(); col_it != row_it.end(); ++col_it){
                        rowDotProduct += col_it.value() * v[col_it.index()]; // Compute the dot product
                        row2NormSquared += col_it.value() * col_it.value(); // Compute the squared 2-norm
                    }

                    // Compute the factor to update v
                    T factor = (d[row_it.index()] - rowDotProduct) / row2NormSquared;

                    // Update v
                    for(auto col_it = row_it.begin(); col_it != row_it.end(); ++col_it){
                        v[col_it.index()] += factor * col_it.value();
                    }
                }
            }
    };

    template<class Matrix>
    class ILUDecomposition : public LinearOperator<typename Matrix::value_type> {
        using T = typename Matrix::value_type;
        private:
            const std::shared_ptr<const SparseMatrix<T>> ILU;
            std::shared_ptr<const SparseMatrix<T>> calculateILUDecomposition(const std::shared_ptr<const SparseMatrix<T>> A, const long unsigned int fillInGenerations) {
                typename SparseMatrix<T>::builder LU(A->rowsize(), A->colsize());
                if(fillInGenerations > 0){
                    // initialize LU with values of A
                    for(auto row_it = A->begin(); row_it != A->end(); ++row_it){
                        for(auto col_it = row_it.begin(); col_it != row_it.end(); ++col_it){
                            LU[row_it.index()][col_it.index()] = T{};
                        }
                    }
                
                    // First phase: create new entries for given fill in generations
                    // for all rows k except the last one
                    for (int k = 0; k < LU.rowsize() - 1; ++k) {
                        // update column k by deviding each element by the diagonal value
                        for (int i = k + 1; i < LU.rowsize(); ++i) {
                            const auto ik = LU[i].find(k); //copy to avoid multiple search operations
                            if (ik != LU[i].end()) {
                                // go over all elements j from i on in row k
                                for (auto j = LU[k].find(i); j != LU[k].end(); ++j) {
                                    const auto a = std::max(j->second, ik->second); //copy to avoid multiple search operations
                                    auto ij = LU[i].find(j->first);
                                    if(a < fillInGenerations && (ij == LU[i].end() || ij->second > (a+1)))  LU[i][j->first] = a + 1; 
                                    
                                }  
                            }
                        } 
                        // go over all elements i from k + 1 on in row k
                        for (int i = k + 2; i < LU.rowsize(); ++i) { 
                            auto ik = LU[i].find(k);
                            if(ik != LU[i].end()){
                                for (auto j = k+1; j < i; ++j) {
                                    auto kj = LU[k].find(j);
                                    if (kj != LU[k].end()){
                                        const auto a = std::max(ik->second, kj->second);
                                        auto ij = LU[i].find(j);
                                        if(a < fillInGenerations && (ij == LU[i].end() || ij->second > (a+1))) LU[i][j] = a + 1;  // 
                                    }
                                        
                                }  
                            }
                        }
                    }
                    // remove all values in LU
                    for(auto row_it = 0; row_it < LU.rowsize(); ++row_it){
                        for(auto col_it = LU[row_it].begin(); col_it != LU[row_it].end(); ++col_it){
                            col_it->second = T{};
                        }
                    }
                }
                // Second phase: calculate values
                // initialize LU with values of A
                for(auto row_it = A->begin(); row_it != A->end(); ++row_it){
                    for(auto col_it = row_it.begin(); col_it != row_it.end(); ++col_it){
                        LU[row_it.index()][col_it.index()] = col_it.value();
                    }
                }

            
                // for all rows k except the last one
                for (int k = 0; k < LU.rowsize() - 1; ++k) {
                    // update column k by deviding each element by the diagonal value
                    for (int i = k + 1; i < LU.rowsize(); ++i) {
                        const auto ik = LU[i].find(k); //copy to avoid multiple search operations
                        if (ik != LU[i].end()) {
                            ik->second /= LU[k][k];
                            // go over all elements j from i on in row k
                            for (auto j = LU[k].find(i); j != LU[k].end(); ++j) {
                                const auto a = j->second * ik->second; //copy to avoid multiple search operations
                                const auto ij = LU[i].find(j->first);
                                if(ij != LU[i].end()){
                                    ij->second -= a;
                                }
                                
                            }  
                        }
                    } 
                    // go over all elements i from k + 1 on in row k
                    for (int i = k + 2; i < LU.rowsize(); ++i) { 
                        auto ik = LU[i].find(k);
                        if(ik != LU[i].end()){
                            for (auto j = k+1; j < i; ++j) {
                                auto ij = LU[i].find(j); 
                                if (ij != LU[i].end()){
                                    auto kj = LU[k].find(j);
                                    if (kj != LU[k].end()){
                                        auto a = ik->second * kj->second;
                                        ij->second -= a;
                                    }    
                                }
                                
                            }  
                        }        
                    }
                }
                //std::cout << "Iterations: " << iterations << std::endl;
                // build LU and return it
                return std::make_shared<const SparseMatrix<T>>(LU.build());
            }

        public:
            // Constructor
            ILUDecomposition(std::shared_ptr<const SparseMatrix<T>> A, const long unsigned int fillInGenerations) : ILU(calculateILUDecomposition(A, fillInGenerations)){}
                
            void apply(const Vector<T>& b, Vector<T>& x) const override {
            
                x.resize((*ILU).rowsize());
                Vector<double> y((*ILU).rowsize());

                // Vorwärtseinsetzen (L * y = b)
                for (auto i = (*ILU).begin(); i != (*ILU).end(); ++i) {
                    T sum = 0;
                    for (auto j = i.begin(); j != i.end(); ++j) {
                        if (j.index()>=i.index()) continue;
                        sum += j.value() * y[j.index()];
                    }
                    y[i.index()] = b[i.index()] - sum;  
                }

                // Rückwärtseinsetzen (U * x = y)
                for (auto rit = (*ILU).rbegin(); rit != (*ILU).rend(); ++rit) {
                    T sum = 0;
                    for (auto j = rit.begin(); j != rit.end(); ++j) {
                        if (j.index()<= rit.index()) continue;
                        sum += j.value() * x[j.index()];
                    }
                    x[rit.index()] = 1/(*ILU)(rit.index(), rit.index()) * (y[rit.index()] - sum);  // Hier ist (*ILU)[i][i] ein Wert der oberen Dreiecksmatrix U
                }
            }
    };

} // namespace hdnum

#endif // SOLVINGLSE_PRECONDITIONERS_HH