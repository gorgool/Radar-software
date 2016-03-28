#include "Utils/EigenHeaders.h"
#include "CoordinateSystems/CSUtils.h"
#include "Shared/SearchArea.h"

namespace TargetEnvironment
{
  // Check if target is within range of operation. Work only for sphere sector search area configuration.
  bool sector_check(const CSUtils::ECEFPoint& target_point, const TargetEnvironment::SearchArea& params, const CSUtils::GCSPoint& radar_location)
  {
    // Conver target point into ENU coordinates
    auto enu_tp = CSUtils::ecef_to_enu(target_point, CSUtils::GCSPoint(CSUtils::degree_to_rad(radar_location.latitude), CSUtils::degree_to_rad(radar_location.longitude)));

    // Conver target point into Spheric coordinates
    auto sph_tp = CSUtils::enu_to_sph(enu_tp);

    // Check range
    if (sph_tp.R > params.range)
      return false;

    // Check angles
    if (std::abs(sph_tp.theta - params.elevation_angle) > params.elevation_width &&
      std::abs(sph_tp.theta - params.azimuth_angle) > params.azimuth_width)
      return false;

    return true;
  };
}
