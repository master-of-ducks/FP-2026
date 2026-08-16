// -*- tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
#ifndef DIFFUSIONMATRIX_HH
#define DIFFUSIONMATRIX_HH

#include "sparsematrix.hh"

/*
* A function that produces a sparse matrix discretizing the
* diffusion operator -\nabla \cdot (c(x) \nabla ... )
* with cell centered finite volumes in any dimension d on a structured grid with n
* cells per direction.
*/
template <typename Number, typename CoefficientFunction, typename BoundaryTypeFunction>
hdnum::SparseMatrix<Number> diffusion_matrix (int n, int d, 
   CoefficientFunction diffusion_coefficient, BoundaryTypeFunction dirichlet_boundary)
{
  std::vector<std::size_t> sizes(d+1,1);
  for (int i=1; i<=d; ++i) sizes[i] = sizes[i-1]*n;
  Number mesh_size = 1.0/n;

  auto builder = typename hdnum::SparseMatrix<Number>::builder(sizes[d], sizes[d]);
  //auto builder = typename hdnum::SparseMatrix<Number>::builder(n, n);

  for (std::size_t index=0; index<sizes[d]; index++)
  //for (std::size_t index=0; index<n; index++)
  {
    // create multiindex from flat index
    std::vector<std::size_t> multiindex(d,0);
    auto copiedindex=index;
    for (int i=d-1; i>=0; i--)
    {
      multiindex[i] = copiedindex/sizes[i];
      copiedindex = copiedindex%sizes[i];
    }
    // std::cout << "index=" << index;
    // for (int i=0; i<d; ++i) std::cout << " " << multiindex[i];
    // std::cout << std::endl;

    // the current cell
    std::vector<double> center_position(d);
    for (int i=0; i<d; ++i) 
      center_position[i] = multiindex[i]*mesh_size;
    double center_coefficient = diffusion_coefficient(center_position);
    double center_matrix_entry = 0.0;

    // loop over all neighbors
    for (int i=0; i<d; i++)
    {
      // down neighbor
      if (multiindex[i]>0)
      {
        // we have a neighbor cell
        std::vector<double> neighbor_position(center_position);
        neighbor_position[i] -= mesh_size;
        double neighbor_coefficient = diffusion_coefficient(neighbor_position);
        double harmonic_average = 2.0/( (1.0/neighbor_coefficient) + (1.0/center_coefficient) );
        builder.addEntry(index,index-sizes[i],-harmonic_average);
        center_matrix_entry += harmonic_average;
      }
      else
      {
        // current cell is on the boundary in this direction
        std::vector<double> neighbor_position(center_position);
        neighbor_position[i] = 0.0;
        if (dirichlet_boundary(neighbor_position))
          center_matrix_entry += center_coefficient*2.0;
      }

      // up neighbor
      if (multiindex[i]<n-1)
      {
        // we have a neighbor cell
        std::vector<double> neighbor_position(center_position);
        neighbor_position[i] += mesh_size;
        double neighbor_coefficient = diffusion_coefficient(neighbor_position);
        double harmonic_average = 2.0/( (1.0/neighbor_coefficient) + (1.0/center_coefficient) );
        builder.addEntry(index,index+sizes[i],-harmonic_average);
        center_matrix_entry += harmonic_average;
      }
      else
      {
        // current cell is on the boundary in this direction
        std::vector<double> neighbor_position(center_position);
        neighbor_position[i] = 1.0;
        if (dirichlet_boundary(neighbor_position))
          center_matrix_entry += center_coefficient*2.0;
      }

    }

    // finally the diagonal entry
    builder.addEntry(index,index,center_matrix_entry);
  }
  return builder.build();
}

#endif
