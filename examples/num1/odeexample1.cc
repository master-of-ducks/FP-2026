#include <iostream>
#include <vector>
#include "hdnum.hh"

using namespace hdnum;

/** @brief Solve linear initial value problem
*/
int main (int argc, char** argv)
{
  // choose a number type
  using Number = double;

  // define the components of the IVP
  Number t0 = 0.0;
  Vector<Number> x0 = {1.0,2.0};
  DenseMatrix<Number> A = {{0.0,1.0},{-1.0,0.0}};
  auto f_lambda = [&](const Number& t, const Vector<Number>& x) { return A*x; };
  auto f_x_lambda = [&](const Number& t, const Vector<Number>& x) {return A; };
 
  // Set up the model to be solved
  auto model = make_model(t0,x0,f_lambda,f_x_lambda); // instantiate model from lambdas
  using Model = decltype(model);      

  // For an implicit method we need a solver for the nonlinear problem
  using Solver = Newton;
  Solver newton;
  newton.set_maxit(20);
  newton.set_verbosity(0);    
  newton.set_reduction(1e-12);
  newton.set_abslimit(1e-15);
  newton.set_linesearchsteps(3);  

  // Now select the ODE solver
  EE<Model> solver1(model);               // this sets the initial value           
  IE<Model,Solver> solver2(model,newton);
  DIRK<Model,Solver> solver3(model,newton,"Alexander");
  auto odesolver = solver3; // chose a solver        

  // set size of the time step
  Number dt = 0.1;
  odesolver.set_dt(dt);

  // allocate storage to save the numerical solution for postprocessing
  std::vector<Number> times;           // store time values here
  std::vector<Vector<Number>> states; // store states here
  std::vector<Number> dts;             // store delta t
  times.push_back(odesolver.get_time());  // initial time
  states.push_back(odesolver.get_state()); // initial state
  dts.push_back(odesolver.get_dt());      // initial dt

  // now do the time steps
  // we assume that no error occurs during the steps
  Number Tend = 50.0;
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

  gnuplot("output.dat",times,states,dts); // output model result

  return 0;
}
