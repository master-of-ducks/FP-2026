#ifndef ILU_DECOMPOSITION_HH
#define ILU_DECOMPOSITION_HH
#include "../hdnum.hh"
#include <stdio.h>
#include <map>
#include <cmath>
#include <memory>

namespace hdnum{


template <class T>
std::shared_ptr<const SparseMatrix<T>> calculateILUDecomposition(const std::shared_ptr<const SparseMatrix<T>> A, T drop_tol = 1e-12) {
    typename SparseMatrix<T>::builder LU(A->rowsize(), A->colsize());

    // initialize LU with values of A
    for(auto row_it = A->begin(); row_it != A->end(); ++row_it){
      for(auto col_it = row_it.begin(); col_it != row_it.end(); ++col_it){
        LU[row_it.index()][col_it.index()] = col_it.value();
      }
    }

    int iterations = 0;
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
                        if (abs(a) > drop_tol) ij->second -= a;
                    }
                    ++iterations;
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
                            if (abs(a) > drop_tol || LU[i].find(j) != LU[i].end()) LU[i][j] -= a;
                        }    
                    }
                    ++iterations;
                }  
            }        
        }
    }
    std::cout << "Iterations: " << iterations << std::endl;
    // build LU and return it
    return std::make_shared<const SparseMatrix<T>>(LU.build());
};
} // namespace hdnum
#endif // ILU_DECOMPOSITION_HH