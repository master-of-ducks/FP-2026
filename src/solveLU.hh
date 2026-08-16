#ifndef SOLVELU_HH
#define SOLVELU_HH
#include <memory>
#include <vector>
#include <map>
#include "../hdnum.hh"

using namespace hdnum;

template<typename T>
void solve_lu(const std::shared_ptr<const SparseMatrix<T>>& LU, const Vector<T>& b, Vector<T>& x) {
    x.resize((*LU).rowsize());
    Vector<double> y((*LU).rowsize());

     // Vorwärtseinsetzen (L * y = b)
    for (auto i = (*LU).begin(); i != (*LU).end(); ++i) {
        T sum = 0;
        for (auto j = i.begin(); j != i.end(); ++j) {
            if (j.index()>=i.index()) continue;
            sum += j.value() * y[j.index()];
        }
        y[i.index()] = b[i.index()] - sum;  
    }

    // Rückwärtseinsetzen (U * x = y)
    for (auto rit = (*LU).rbegin(); rit != (*LU).rend(); ++rit) {
        T sum = 0;
        for (auto j = rit.begin(); j != rit.end(); ++j) {
            if (j.index()<= rit.index()) continue;
            sum += j.value() * x[j.index()];
        }
        x[rit.index()] = 1/(*LU)(rit.index(), rit.index()) * (y[rit.index()] - sum);  // Hier ist (*LU)[i][i] ein Wert der oberen Dreiecksmatrix U
    }
}
#endif // SOLVELU_HH