#include <iostream>    // notwendig zur Ausgabe
#include "hdnum.hh"    // hdnum header


/*
* Sample code testing iterative solvers
*/
int main ()
{
  using Number = double; // choose your number type
  using Matrix = hdnum::SparseMatrix<Number>;
  using Vector = hdnum::Vector<double>;

  // generate a matrix discretizing a diffusion problem
  auto diffusion_coefficient = [](const std::vector<double>& x) { return 1.0; };
  auto dirichlet_boundary = [](const std::vector<double>& x) { return true; };
  auto pA = std::make_shared<Matrix>(diffusion_matrix<double>(20,2,diffusion_coefficient,dirichlet_boundary));

  // sparse matrix vector product
  auto px = std::make_shared<Vector>(pA->colsize(),1.0);    // make two vectors
  auto pb = std::make_shared<Vector>(pA->colsize());    // make two vectors
  pA->mv(*pb,*px);

  // unpreconditioned loop solver
  hdnum::LoopSolver<Matrix> solver1(pA,100000,1e-6,0.2);
  *px = 0.0;
  solver1.apply(*pb,*px);
  std::cout << "unpreconditioned LoopSolver needed " << solver1.getDefects().size() << " iterations" << std::endl;

  // set up ilu preconditioner
  auto  pilu = std::make_shared<hdnum::ILUDecomposition<Matrix>>(pA,0);

  // solve using ilu in loop solver
  hdnum::LoopSolver<Matrix> solver2(pA,pilu,10000,1e-6);
  *px = 0.0;
  solver2.apply(*pb,*px);
  std::cout << "ILU preconditioned LoopSolver needed " << solver2.getDefects().size() << " iterations" << std::endl;

  // solve using ilu in CG
  hdnum::PCG<Matrix> solver3(pA,pilu,10000,1e-6);
  *px = 0.0;
  solver3.apply(*pb,*px);
  std::cout << "ILU preconditioned CG needed " << solver3.getDefects().size() << " iterations" << std::endl;
}