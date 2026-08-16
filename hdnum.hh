// -*- tab-width: 4; indent-tabs-mode: nil -*-
#ifndef HDNUM_HDNUM_HH
#define HDNUM_HDNUM_HH

#define HDNUM_DEVEL_MODE 1

#include <math.h>

#include <complex>

#if HDNUM_HAS_GMP
#include <gmpxx.h>
#include "src/highprec_gmp.hh"
#endif

#ifdef HDNUM_HAS_CPFLOAT
#include "src/lowprec_cpfloat.hh"
#endif

// define floating point types
namespace hdnum {
#if HDNUM_HAS_GMP
  using FP1024 = FP<960>;
  using FP512 = FP<448>;
  using FP256 = FP<192>;
  using FP128 = FP<64>;
#endif
  using FP64 = double;
  using FP32 = float;
#ifdef HDNUM_HAS_CPFLOAT
  using FP16 = hdnum::CPFloat<11,5>;
  using bfloat16 = hdnum::CPFloat<8,8>;
  using FP8 = hdnum::CPFloat<4,4>;
#endif
}


// general utilities
#include "src/densematrix.hh"
#include "src/exceptions.hh"
#include "src/opcounter.hh"
#include "src/precision.hh"
#include "src/sparsematrix.hh"
#include "src/diffusionmatrix.hh"
#include "src/timer.hh"
#include "src/vector.hh"

// Num0
#include "src/lr.hh"
#include "src/newton.hh"
#include "src/qr.hh"

// Num1
#include "src/ode.hh"
#include "src/pde.hh"
#include "src/rungekutta.hh"
#include "src/sgrid.hh"

// PracticalsSS2024
#include "src/SolvingLSE-Methods.hh"
#include "src/SolvingLSE-Preconditioners.hh"
#include "src/LinearOperator.hh"
#endif
