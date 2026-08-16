#ifndef DIRECTSOLVERS_HH
#define DIRECTSOLVERS_HH

#include <iomanip>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>
#include "LinearOperator.hh"
#include "luDecomposition.hh"
#include "iluDecomposition.hh"
#include "solveLU.hh"
using namespace hdnum;
template<class T>
    class LUDecomposition : public LinearOperator<T> {
        private:
            const std::shared_ptr<const SparseMatrix<T>> LU;
        
        public:
            LUDecomposition(std::shared_ptr< const SparseMatrix<T>> A, T drop_tolerance) : LU(calculateLUDecomposition(A, drop_tolerance)) {}
            
            void apply(const Vector<T>& in, Vector<T>& out) const {
                solve_lu(LU, in, out);
            }
    };
    template<class T>
    class ILUDecomposition : public LinearOperator<T> {
        private:
            const std::shared_ptr<const SparseMatrix<T>> ILU;
        
        public:
            ILUDecomposition(std::shared_ptr<const SparseMatrix<T>> A, T drop_tolerance) : ILU(calculateILUDecomposition(A, drop_tolerance)) {}

            void apply(const Vector<T>& in, Vector<T>& out) const {
                solve_lu(ILU, in, out);
            }
    };
#endif // DIRECTSOLVERS_HH