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

    // Geodetic coordinates, degrees
    double latitude;
    double longitude;
    double height;

    // Max range of scanning and tracking, meters
    double range;

    // Working area description, dergees
    double elevation_ang;
    double elevation_width;

    double azimuth_ang;
    double azimuth_width;

    ReferencePointDesc(std::size_t id, const double lat, const double lon, const double h, const double r,
      const double elev_ang, const double elev_width, const double azim_ang, const double azim_width) :
      client_id(id),
      latitude(lat),
      longitude(lon),
      height(h),
      range(r),
      elevation_ang(elev_ang),
      elevation_width(elev_width),
      azimuth_ang(azim_ang),
      azimuth_width(azim_width)
      {}

    ReferencePointDesc() = default;
  };
}