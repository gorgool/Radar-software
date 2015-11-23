#include "../../../Utils/include/EigenHeaders.h"
#include "../../../CoordinateSystems/include/CSUtils.h"
#include "../../Shared/include/ReferencePointDesc.h"

namespace TargetEnvironment
{
  // Check if target is within range of operation
  bool sector_check(const Eigen::Vector3d& target_point, const ReferencePointDesc& params)
  {
    // Conver target point into ENU coordinates
    CSUtils::ECEFPoint ecef_tp(target_point(0), target_point(1), target_point(2));
    auto enu_tp = CSUtils::ecef_to_enu(ecef_tp, CSUtils::GCSPoint(CSUtils::degree_to_rad(params.latitude), CSUtils::degree_to_rad(params.longitude)));

    // Conver target point into Spheric coordinates
    auto sph_tp = CSUtils::enu_to_sph(enu_tp);
    
    // Check range
    if (sph_tp.R > params.range)
      return false;

    // Check angles
    if (std::abs(sph_tp.theta - params.elevation_ang) > params.elevation_width &&
      std::abs(sph_tp.theta - params.azimuth_ang) > params.azimuth_width)
      return false;

    return true;
  };
}
