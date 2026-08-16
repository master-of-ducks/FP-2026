// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
/*
 * File:   SolvingLSE-Methods.hh
 * Author: Thomas Gantz <thomas.gantz@stud.uni-heidelberg.de>
 *
 * Created on August 03, 2024
 */

#include "LinearOperator.hh"
#include "SolvingLSE-Preconditioners.hh"
#include "vector.hh"
#include <memory> // For std::shared_ptr
#include <vector> // For std::vector

#ifndef SOLVINGLSE_METHODS_HH
#define SOLVINGLSE_METHODS_HH

namespace hdnum {

    // Abstract base class DefectTracker
    template<class T>
    class DefectTracker {
    public:
        // Virtual destructor for proper cleanup of derived classes
        virtual ~DefectTracker() = default;
        // Pure virtual function to get defects
        virtual const std::vector<T>& getDefects() const = 0;
    };

    // Combined base class Method
    template<class T>
    class Method : public LinearOperator<T>, public DefectTracker<T> {
    public:
        // Virtual destructor for proper cleanup of derived classes
        virtual ~Method() = default;
    };

    // Preconditioned Gradient Descent Method
    template<class Matrix>
    class PGD : public Method<typename Matrix::value_type> {

    public:
        using T = typename Matrix::value_type;

    private:
        std::shared_ptr<LinearOperator<T>> preconditioner;  // Preconditioner for the method
        std::shared_ptr<Matrix> A;  // Coefficient matrix
        const std::size_t max_it;  // Maximum number of iterations
        const T epsilon;  // Convergence tolerance
        mutable std::vector<T> defects;  // Store defect norms for each iteration

    public:
        // Constructor with a provided preconditioner
        PGD(std::shared_ptr<Matrix> A_, std::shared_ptr<LinearOperator<T>> preconditioner_, const std::size_t max_it_, const T epsilon_)
            : preconditioner(preconditioner_), A(A_), max_it(max_it_), epsilon(epsilon_) {}

        // Constructor with default Richardson preconditioner
        PGD(std::shared_ptr<Matrix> A_, const std::size_t max_it_, const T epsilon_)
            : preconditioner(std::make_shared<Richardson<T>>(1000)), A(A_), max_it(max_it_), epsilon(epsilon_) {}

        // Getter for defects
        const std::vector<T>& getDefects() const override { return defects; }

        // Apply method to solve the linear system Ax = b
        virtual void apply(const Vector<T>& b, Vector<T>& x) const override {

            // Initialize defects vector
            defects = {};

            // Compute the initial residual d = b - A*x0
            Vector<T> d(x.size());
            Vector<T> temp(x.size());
            A->mv(temp, x);  // temp = A*x0
            for (std::size_t i = 0; i < x.size(); ++i) {
                d[i] = b[i] - temp[i];
            }

            // Compute initial defect norm and set stopping criteria
            T delta = norm(d);
            T delta0 = delta * epsilon;

            // Iteration counter
            std::size_t it = 0;

            // Iterative loop until convergence or maximum iterations
            while (delta > delta0 && it < max_it) {
                
                // Apply preconditioner to the residual
                Vector<T> v(x.size(), T{});
                preconditioner->apply(d, v);

                // Compute A*v
                Vector<T> q(x.size());
                A->mv(q, v);

                // Compute the step size alpha = (d, v) / (v, q)
                T alphaNum = 0;
                T alphaDen = 0;
                for (std::size_t i = 0; i < x.size(); ++i) {
                    alphaNum += d[i] * v[i];
                    alphaDen += v[i] * q[i];
                }
                T alpha = alphaNum / alphaDen;

                // Update the solution and the residual
                for (std::size_t i = 0; i < x.size(); ++i) {
                    x[i] += alpha * v[i];
                    d[i] -= alpha * q[i];
                }

                // Compute new defect norm
                delta = norm(d);
                // Store defect norm for each iteration
                defects.push_back(delta);

                // Increment iteration counter
                ++it;
            }
        }
    };

    // Preconditioned Conjugate Gradient Method
    template<class Matrix>
    class PCG : public Method<typename Matrix::value_type> {

    public:
        using T = typename Matrix::value_type;

    private:
        std::shared_ptr<LinearOperator<T>> preconditioner;  // Preconditioner for the method
        std::shared_ptr<Matrix> A;  // Coefficient matrix
        const std::size_t max_it;  // Maximum number of iterations
        const T epsilon;  // Convergence tolerance
        mutable std::vector<T> defects;  // Store defect norms for each iteration

    public:
        // Constructor with a provided preconditioner
        PCG(std::shared_ptr<Matrix> A_, std::shared_ptr<LinearOperator<T>> preconditioner_, const std::size_t max_it_, const T epsilon_)
            : preconditioner(preconditioner_), A(A_), max_it(max_it_), epsilon(epsilon_) {}

        // Constructor with default Richardson preconditioner
        PCG(std::shared_ptr<Matrix> A_, const std::size_t max_it_, const T epsilon_)
            : preconditioner(std::make_shared<Richardson<T>>(10)), A(A_), max_it(max_it_), epsilon(epsilon_) {}

        // Getter for defects
        const std::vector<T>& getDefects() const override { return defects; }

        // Apply method to solve the linear system Ax = b
        virtual void apply(const Vector<T>& b, Vector<T>& x) const override {

            // Initialize defects vector
            defects = {};

            // Compute the initial residual d = b - A*x0
            Vector<T> d(x.size());
            Vector<T> p(x.size());
            Vector<T> temp(x.size());
            A->mv(temp, x);  // temp = A*x0
            for (std::size_t i = 0; i < x.size(); ++i) {
                d[i] = b[i] - temp[i];
                p[i] = d[i];  // p initialized to d
            }

            // Compute initial defect norm and set stopping criteria
            T delta = norm(d);
            T delta0 = delta * epsilon;

            // Iteration counter
            std::size_t it = 0;

            // Iterative loop until convergence or maximum iterations
            while (delta > delta0 && it < max_it) {

                // Apply preconditioner to the search direction
                Vector<T> v(x.size(), T{});
                preconditioner->apply(p, v);

                // Compute A*v
                Vector<T> q(x.size());
                A->mv(q, v);

                // Compute the step size alpha = (d, v) / (v, q)
                T alphaNum = 0;
                T alphaDen = 0;
                for (std::size_t i = 0; i < x.size(); ++i) {
                    alphaNum += d[i] * v[i];
                    alphaDen += v[i] * q[i];
                }
                T alpha = alphaNum / alphaDen;

                // Update the solution and the residual
                for (std::size_t i = 0; i < x.size(); ++i) {
                    x[i] += alpha * v[i];
                    d[i] -= alpha * q[i];
                }

                // Compute the coefficient beta = (d, q) / (v, q)
                T betaNum = 0;
                for (std::size_t i = 0; i < x.size(); ++i) {
                    betaNum += d[i] * q[i];
                }
                T beta = betaNum / alphaDen;

                // Update the search direction
                for (std::size_t i = 0; i < x.size(); ++i) {
                    p[i] = d[i] - beta * p[i];
                }

                // Compute new defect norm
                delta = norm(d);
                // Store defect norm for each iteration
                defects.push_back(delta);

                // Increment iteration counter
                ++it;
            }
        }
    };

    // LoopSolver class with Preconditioner
    template<class Matrix>
    class LoopSolver : public Method<typename Matrix::value_type> {

    public:
        using T = typename Matrix::value_type;

    private:
        std::shared_ptr<LinearOperator<T>> preconditioner;  // Preconditioner for the method
        std::shared_ptr<Matrix> A;  // Coefficient matrix
        const std::size_t max_it;  // Maximum number of iterations
        const T epsilon;  // Convergence tolerance
        mutable std::vector<T> defects;  // Store defect norms for each iteration
        const T alpha;  // Step size

    public:
        // Constructor with a provided preconditioner
        LoopSolver(std::shared_ptr<Matrix> A_, std::shared_ptr<LinearOperator<T>> preconditioner_, const std::size_t max_it_, const T epsilon_, const T alpha_ = T(1))
            : preconditioner(preconditioner_), A(A_), max_it(max_it_), epsilon(epsilon_), alpha(alpha_) {}

        // Constructor with default Richardson preconditioner
        LoopSolver(std::shared_ptr<Matrix> A_, const std::size_t max_it_, const T epsilon_, const T alpha_ = T(1))
            : preconditioner(std::make_shared<Richardson<T>>(10)), A(A_), max_it(max_it_), epsilon(epsilon_), alpha(alpha_) {}

        // Getter for defects
        const std::vector<T>& getDefects() const override { return defects; }

        // Apply method to solve the linear system Ax = b
        virtual void apply(const Vector<T>& b, Vector<T>& x) const override {

            // Initialize defects vector
            defects = {};

            // Compute the initial residual d = b - A*x0
            Vector<T> d(x.size());
            Vector<T> temp(x.size());
            A->mv(temp, x);  // temp = A*x0
            for (std::size_t i = 0; i < x.size(); ++i) {
                d[i] = b[i] - temp[i];
            }

            // Compute initial defect norm and set stopping criteria
            T delta = norm(d);
            T delta0 = delta * epsilon;

            // Iteration counter
            std::size_t it = 0;

            // Iterative loop until convergence or maximum iterations
            while (delta > delta0 && it < max_it) {

                // Apply preconditioner to the residual
                Vector<T> v(x.size(), T{});
                preconditioner->apply(d, v);

                // Compute A*v
                Vector<T> q(x.size());
                A->mv(q, v);

                // Update the solution and the residual
                for (std::size_t i = 0; i < x.size(); ++i) {
                    x[i] += alpha * v[i];
                    d[i] -= alpha * q[i];
                }

                // Compute new defect norm
                delta = norm(d);
                // Store defect norm for each iteration
                defects.push_back(delta);

                // Increment iteration counter
                ++it;
            }
        }
    };
    
    template<class T>
    class LUDecomposition : public LinearOperator<T> {
        private:
            const std::shared_ptr<const SparseMatrix<T>> LU;
            std::shared_ptr<const SparseMatrix<T>> calculateLUDecomposition(const std::shared_ptr<const SparseMatrix<T>> A) {
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
                            ++iterations;
                            // go over all elements j from i on in row k
                            for (auto j = LU[k].find(i); j != LU[k].end(); ++j) {
                                const auto a = j->second * ik->second; //copy to avoid multiple search operations
                                LU[i][j->first] -= a; // if (abs(a) > drop_tol || LU[i].find(j->first) != LU[i].end())
                                ++iterations;
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
                                    auto a = ik->second * kj->second;
                                    LU[i][j] -= a;  // if (abs(a) > drop_tol || LU[i].find(j) != LU[i].end()) 
                                }
                                ++iterations;     
                            }  
                        }
                    }
                }
                std::cout << "Iterations: " << iterations << std::endl;
                // build LU and return it
                
                return std::make_shared<const SparseMatrix<T>>(LU.build());
            }
        
        public:
            LUDecomposition(std::shared_ptr<const SparseMatrix<T>> A) : LU(calculateLUDecomposition(A)) {}
            
            
            void apply(const Vector<T>& b, Vector<T>& x) const {
                x.resize((*LU).rowsize());
                Vector<double> y((*LU).rowsize());

                // foreward substitution (L * y = b)
                for (auto i = (*LU).begin(); i != (*LU).end(); ++i) {
                    T sum = 0;
                    for (auto j = i.begin(); j != i.end(); ++j) {
                        if (j.index()>=i.index()) continue;
                        sum += j.value() * y[j.index()];
                    }
                    y[i.index()] = b[i.index()] - sum;  
                }

                // backward substitution (U * x = y)
                for (auto rit = (*LU).rbegin(); rit != (*LU).rend(); ++rit) {
                    T sum = 0;
                    for (auto j = rit.begin(); j != rit.end(); ++j) {
                        if (j.index()<= rit.index()) continue;
                        sum += j.value() * x[j.index()];
                    }
                    x[rit.index()] = 1/(*LU)(rit.index(), rit.index()) * (y[rit.index()] - sum); 
                }
            }
    };
} // namespace hdnum

#endif // SOLVINGLSE_METHODS_HH

