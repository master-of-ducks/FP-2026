#include <iostream>
#include <gmpxx.h>
#include "hdnum.hh"

using namespace hdnum;

//using number = FP256;
using number = double;

void test1 ()
{
  const int n=7;
  Vector<number> x(n);
  Vector<number> b(n);
  Vector<number> s(n);
  Vector<std::size_t> p(n);
  Vector<std::size_t> q(n);
  DenseMatrix<number> A(n,n);
  fill(x,number(1.0),number(1.0));
  vandermonde(A,x);
  A.mv(b,x);
  x = number(0.0);
  row_equilibrate(A,s);
  lr_fullpivot(A,p,q);
  apply_equilibrate(s,b);
  permute_forward(p,b);
  solveL(A,b,b);
  solveR(A,x,b);
  permute_backward(q,x);
  std::cout << x << std::endl;
}

void test2 ()
{
  // set up linear system
  int n;
  std::cout << "enter size (positive integer)" << std::endl;
  std::cin >> n;
  Vector<number> x(n);
  fill(x,number(1.0),number(1.0));
  Vector<number> b(n);
  Vector<std::size_t> p(n);
  DenseMatrix<number> A(n,n);
  for (int i=0; i<n; i++)
    for (int j=0; j<n; j++)
      A[i][j] = 1.0/((1.0+std::abs(i-j))*(1.0+std::abs(i-j)));
  A.mv(b,x);
  x = number(0.0);

  // Factorize matrix
  Timer timer;
  lr_partialpivot(A,p);
  auto elapsed = timer.elapsed();
  std::cout << "time for factorization " << elapsed << " seconds" << std::endl;
  std::cout << "performance in factorization " << (0.666*n*n*n+1.5*n*n)/elapsed/1e9 << " Gigaflops per second" << std::endl;

  // Solve linear system
  timer.reset();
  permute_forward(p,b);
  solveL(A,b,b);
  solveR(A,x,b);
  elapsed = timer.elapsed();
  std::cout << "time for backsolve " << elapsed << " seconds" << std::endl;  
}

int main ()
{
  test1();
  test2();
  return 0;
}
