#include "../include/ComputeParams.h"

// Precumputed 6x6 matrix for 6 time points - 0 10 20 30 40 50
// x_prec = ((X'*X)^-1)*X', where ' - transpose matrix, ^-1 reverse matrix
const double x_data[] =
{
  1.0, 1.05723901855370e-11, -1.97386551548107e-11, 1.77693415537306e-11, -7.87281351222191e-12, 1.37845290737459e-12,
  -2.283333333e-1, 0.5, -0.5, 0.333333333, -0.125, 0.02,
  3.75e-2, -1.28333333e-1, 1.78333333e-1,  -0.13, 5.08333333e-2, -8.3333333e-3,
  -4.25e-3, 1.775e-2, -2.95e-2, 0.0245, -1.025e-2, 1.75e-3,
  3.0e-4, -1.4e-3, 2.6e-003, -2.4e-3, 1.1e-3, -2.0e-4,
  -1.0e-5, 5.0e-5, -1.0e-4, 1.0e-4, -5.0e-5, 1.0e-5
};
const static Eigen::Matrix<double, 6, 6> x_prec(x_data);

static std::array<double, 6> comp_par_helper(const std::array<double, 6>& points)
{
  Eigen::Matrix<double, 6, 1> p;
  p(0) = points[0];
  p(1) = points[1];
  p(2) = points[2];
  p(3) = points[3];
  p(4) = points[4];
  p(5) = points[5];

  auto params = x_prec * p;

  return {params(0), params(1), params(2), params(3), params(4), params(5)};
}

TargetEnvironment::TargetDesc compute_params(
  const std::size_t id,
  const std::array<double, 6>& points_x, 
  const std::array<double, 6>& points_y, 
  const std::array<double, 6>& points_z)
{
  return TargetEnvironment::TargetDesc(id,
    comp_par_helper(points_x),
    comp_par_helper(points_y),
    comp_par_helper(points_z)
    );
}
