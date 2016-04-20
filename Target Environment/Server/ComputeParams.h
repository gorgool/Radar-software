#pragma once

#include <array>
#include "Shared/Target.h"

/**
* @fn  TargetEnvironment::Target compute_params( const std::uint32_t id, const std::array<double, 6>& points_x, const std::array<double, 6>& points_y, const std::array<double, 6>& points_z)
*
* @brief Fit points into polynomial regression.
*
* @author  Gorgool
* @date  20.04.2016
*
* @param id        The identifier.
* @param points_x  The points x coordinate.
* @param points_y  The points y coordinate.
* @param points_z  The points z coordinate.
*
* @return  Target.
*/

TargetEnvironment::Target compute_params(
  const std::uint32_t id,
  const std::array<double, 6>& points_x,
  const std::array<double, 6>& points_y,
  const std::array<double, 6>& points_z);
