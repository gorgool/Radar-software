#include "../include/ComputeParams.h"

// Precumputed 6x6 matrix for 6 time points - 0 10 20 30 40 50
// x_prec = ((X'*X)^-1)*X', where ' - transpose matrix, ^-1 reverse matrix
// *) In transponse form, Eigen copy data by columns
const double x_data[] =
{
  1.000000000002234, -0.228333333348094, 0.037500000004917, -0.004250000000822, 0.000300000000074, -0.000010000000003,
  -0.000000000007910, 0.500000000076929, -0.128333333351056, 0.017750000003601, -0.001400000000315, 0.000050000000013,
  0.000000000012434, -0.500000000104629, 0.178333333385611, -0.029500000004713, 0.002600000000584, -0.000100000000022,
  -0.000000000010440, 0.333333333430922, -0.130000000007785, 0.024500000011848, -0.002400000000351, 0.000100000000020,
  0.000000000004659, -0.125000000035072, 0.050833333378961, -0.010249999987836, 0.001100000000941, -0.000050000000010,
  -0.000000000000821, 0.020000000012828, -0.008333333295482, 0.001750000025723, -0.000199999997919, 0.000010000000002
};
const static Eigen::Matrix<double, 6, 6> x_prec(x_data);

static std::array<double, 6> comp_par_helper(const std::array<double, 6>& points)
{
  Eigen::Matrix<double, 6, 1> p(points.data());
  Eigen::Matrix<double, 6, 1> params = x_prec * p;
  return {params(0), params(1), params(2), params(3), params(4), params(5)};
}

TargetEnvironment::TargetDesc compute_params(
  const std::uint32_t id,
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
