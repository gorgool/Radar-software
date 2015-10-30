#pragma once

/*
  Placement description of radar station and its working area. 
*/

#include <inttypes.h>

namespace TargetEnvironment
{
  struct ReferencePointDesc
  {
    std::size_t client_id;

    // Geodetic coordinates
    double latitude;
    double longitude;
    double height;

    // Max range of scanning and tracking
    double range;

    ReferencePointDesc(std::size_t id, const double lat, const double lon, const double h, const double r) :
      client_id(id),
      latitude(lat),
      longitude(lon),
      height(h),
      range(r) {}

    ReferencePointDesc() = default;
  };
}