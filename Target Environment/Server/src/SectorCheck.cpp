#include "../../../Utils/include/EigenHeaders.h"
#include "../../../CoordinateSystems/include/CSUtils.h"
#include "../../Shared/include/ReferencePointDesc.h"

namespace TargetEnvironment
{
  // Check if target is within range of operation
  bool sector_check(const Eigen::Vector3d& target_point, const ReferencePointDesc& params)
  {
    // Convert point from Geodetic CS into Geocentric Cartesian CS
    CSUtils::GCSPoint gcs_ref_point(params.latitude, params.longitude, params.height);
    auto ecef_ref_point = CSUtils::gcs_to_ecef(gcs_ref_point);
    Eigen::Vector3d gccs_params(ecef_ref_point.x, ecef_ref_point.y, ecef_ref_point.z);

    // Check range
    auto temp = target_point - gccs_params;
    if (temp.norm() > params.range)
      return false;
    return true;
  };
}
