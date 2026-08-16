#include <iostream>    // notwendig zur Ausgabe
#include "hdnum.hh"    // hdnum header


/*
* Sample code how to work with sparse matrices in hdnum
*/
int main ()
{
  using Number = double; // choose your number type

  // make a sparse matrix from individual entries
  auto builder = typename hdnum::SparseMatrix<Number>::builder(3, 3);
  builder.addEntry(0,0,1.0);
  builder.addEntry(1,1,2.0);
  builder.addEntry(1,2,3.0);
  builder.addEntry(2,2,4.0);
  auto A = builder.build();

  // iterate over nonzero entries of a sparse matrix
  for(auto rowit = A.begin(); rowit != A.end(); ++rowit)
    for(auto colit = rowit.begin(); colit != rowit.end(); ++colit)
    {
      auto value = colit.value();
      auto colindex = colit.index();
      auto rowindex = rowit.index();
      std::cout << "i=" << rowindex << " j=" << colindex << " value=" << value << std::endl;
    }

  // print matrix
  std::cout << A << std::endl;

  // conversion to a dense matrix (with different type)
  hdnum::DenseMatrix<float> AA(A);
  std::cout << AA << std::endl;

  // generate a matrix discretizing a diffusion problem
  auto diffusion_coefficient = [](const std::vector<double>& x) { return 1.0; };
  auto dirichlet_boundary = [](const std::vector<double>& x) { return true; };
  auto B = diffusion_matrix<double>(4,2,diffusion_coefficient,dirichlet_boundary);
  std::cout << B << std::endl;

  // sparse matrix vector product
  hdnum::Vector<float> x(B.colsize(),1.0);    // make two vectors
  hdnum::Vector<float> y(B.rowsize(),0.0);
  B.mv(y,x);                  // y = B*x
}