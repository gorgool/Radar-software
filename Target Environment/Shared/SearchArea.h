#pragma once

namespace TargetEnvironment
{
  /*
    Search area description
  */
  struct SearchArea
  {
    // ENU coordinates of reference point, meters
    double x;
    double y;
    double z;

    // Max range (depth) of scanning and tracking, meters
    double range;

    // Working area description, dergees
    double elevation_angle;
    double elevation_width;

    double azimuth_angle;
    double azimuth_width;

    SearchArea(const double x_pos, const double y_pos, const double z_pos, const double r,
      const double elev_ang, const double elev_width, const double azim_ang, const double azim_width) :
      x(x_pos),
      y(y_pos),
      z(z_pos),
      range(r),
      elevation_angle(elev_ang),
      elevation_width(elev_width),
      azimuth_angle(azim_ang),
      azimuth_width(azim_width)
    {}

    SearchArea() = default;
  };

}