#pragma once

#include "Utils/EigenHeaders.h"
#include "Shared/SearchArea.h"
#include "CoordinateSystems/CSUtils.h"

namespace TargetEnvironment
{
  /**
  * @fn  bool sector_check(const CSUtils::ECEFPoint& target_point, const TargetEnvironment::SearchArea& params, const CSUtils::GCSPoint& radar_location)
  *
  * @brief Check if target is within range of operation. Work only for sphere sector search area configuration.
  *
  * @author  Gorgool
  * @date  20.04.2016
  *
  * @param target_point    Target point.
  * @param params          Search area description.
  * @param radar_location  The radar location.
  *
  * @return  true if it succeeds, false if it fails.
  */

  bool sector_check(const CSUtils::ECEFPoint& target_point, const TargetEnvironment::SearchArea& params, const CSUtils::GCSPoint& radar_location);
}
