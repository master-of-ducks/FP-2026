#include <iostream>
#include <vector>
#include "hdnum.hh"

/** @brief Define a linear scalar initial value problem

    u'(t) = lambda*u(t), t>=t_0, u(t_0) = u_0.

    \tparam T a number type representing time values
    \tparam N a number type used in states and function values
*/
template<class T, class N=T>
class IVP
{
public:
  /** \brief export size_type */
  typedef std::size_t size_type;

  /** \brief export time_type */
  typedef T time_type;

  /** \brief export number_type */
  typedef N number_type;

  //! constructor stores parameter lambda
  IVP (const N& lambda_)
    : lambda(lambda_)
  {}

  //! return number of components for the model
  std::size_t size () const
  {
    // this is a scalar problem
    return 1;
  }

  //! set initial state including time value
  // x0 needs to be allocated to the correct size
  void initialize (T& t0, hdnum::Vector<N>& x0) const
  {
    t0 = 0;
    x0[0] = 1.0;
  }

  //! model evaluation result = t(t,x)
  // results needs to be allocated to the correct size
  void f (const T& t, const hdnum::Vector<N>& x, hdnum::Vector<N>& result) const
  {
    //result[0] = lambda*x[0];
    result[0] = x[0]*x[0];
  }

  //! jacobian evaluation needed for implicit solvers
  // result needs to be allocated to the correct size
  void f_x (const T& t, const hdnum::Vector<N>& x, hdnum::DenseMatrix<N>& result) const
  {
    //result[0][0] = lambda;
    result[0][0] = 2.0*x[0];
  }

  //! exact solution if known
  // result needs to be allocated to the correct size
  void exact_solution (const T& t, hdnum::Vector<N>& result) const
  {
    result[0] = exp(lambda*t);
  }

private:
  N lambda;
};


/** @brief Solve initial value problem
*/
int main (int argc, char** argv)
{
  // Choose number type
  using Number = double;

  // Set up the model to be solved
  using Model = IVP<Number>; // Choose model type
  Model model(-0.5);         // instantiate model

  // For an implicit method we need a solver for the nonlinear problem
  using Solver = hdnum::Newton;
  Solver newton;
  newton.set_maxit(20);
  newton.set_verbosity(0);    
  newton.set_reduction(1e-12);
  newton.set_abslimit(1e-15);
  newton.set_linesearchsteps(3);  

  // Now select the ODE solver: implicit Euler scheme
  hdnum::EE<Model> solver1(model);               // this sets the initial value           
  hdnum::IE<Model,Solver> solver2(model,newton); // this sets the initial value
  auto odesolver = solver1; // chose a solver        

  // set size of the time step
  Number dt = 0.001;
  odesolver.set_dt(dt);

  // allocate storage to save the numerical solution for postprocessing
  std::vector<Number> times;           // store time values here
  std::vector<hdnum::Vector<Number>> states; // store states here
  std::vector<Number> dts;             // store delta t
  times.push_back(odesolver.get_time());  // initial time
  states.push_back(odesolver.get_state()); // initial state
  dts.push_back(odesolver.get_dt());      // initial dt

  // now do the time steps
  // we assume that no error occurs during the steps
  Number Tend = 0.99;
  int steps = 0;
  while (odesolver.get_time()<Tend-1e-8) // the time loop
    {
      odesolver.step();                  // advance model by one time step
      times.push_back(odesolver.get_time());   // save time
      states.push_back(odesolver.get_state()); // and state
      dts.push_back(odesolver.get_dt());       // used dt
      steps++;
    }

  // write some information 
  std::cout << "Solution at time " << odesolver.get_time() 
            << " : " << odesolver.get_state() << std::endl;

  hdnum::gnuplot("output.dat",times,states,dts); // output model result

  return 0;
}
