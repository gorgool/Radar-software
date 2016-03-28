#pragma once

#include "Utils/EigenHeaders.h"
#include "Shared/SearchArea.h"
#include "CoordinateSystems\CSUtils.h"

namespace TargetEnvironment
{
  // Check if target is within range of operation.
  bool sector_check(const CSUtils::ECEFPoint& target_point, const TargetEnvironment::SearchArea& params, const CSUtils::GCSPoint& radar_location);
}
