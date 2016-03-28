#pragma once

#include <array>
#include "Shared/Target.h"

/*
    Compute 5th degree polinom params (using normal equation for polynomial regression)
    for input points. Return TargetDesc.
*/

TargetEnvironment::Target compute_params(
  const std::uint32_t id,
  const std::array<double, 6>& points_x,
  const std::array<double, 6>& points_y,
  const std::array<double, 6>& points_z);
