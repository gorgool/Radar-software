#ifndef CS_TRANSFORM_H
#define CS_TRANSFORM_H

#include "CSPoints.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include "Utils/EigenHeaders.h"

namespace CSUtils
{
  /*
    ог 90.02 Constants (WGS-84)
  */
  const static double a = 6378136.0;
  const static double b = 6356751.36;
  const static double e_2 = 0.0066943662;
  
  struct Angle 
  { 
    double degrees, minutes, seconds; // in radians

    Angle(const double _degrees = 0.0, const double _minutes = 0.0, const double _seconds = 0.0) : degrees(_degrees), minutes(_minutes), seconds(_seconds)
    {
      if (_degrees > 360.0) degrees -= static_cast<unsigned int>(_degrees / 360.0) * 360.0;
      if (_degrees > 60.0) minutes -= static_cast<unsigned int>(_minutes / 60.0) * 60.0;
      if (_degrees > 60.0) seconds -= static_cast<unsigned int>(_seconds / 60.0) * 60.0;
    }
  };

  /*
    Transform precise coordinate from degrees to radian representation and vice versa
  */
  double degree_to_rad(double _degrees, double _minutes = 0.0, double _seconds = 0.0);
  double degree_to_rad(const Angle& _angle);
  Angle rad_to_degrees(double _rads);

  /*
    Coordinate systems transforms
  */

  BCSPoint ccsa_to_bcs(const CCSAPoint& _point);
  CCSAPoint bcs_to_ccsa(const BCSPoint& _point);

  // _eps - elevation angle, _beta - azimuth angle
  ENUPoint ccsa_to_enu(const CCSAPoint& _point, const Angle& _eps, const Angle& _beta);
  CCSAPoint enu_to_ccsa(const ENUPoint& _point, const Angle& _eps, const Angle& _beta);

  SPHPoint enu_to_sph(const ENUPoint& _point);
  ENUPoint sph_to_enu(const SPHPoint& _point);

  // _origin - position of ENU center
  ENUPoint ecef_to_enu(const ECEFPoint& _point, const CSUtils::GCSPoint& _origin);
  ECEFPoint enu_to_ecef(const ENUPoint& _point, const CSUtils::GCSPoint& _origin);
  
  ECEFPoint gcs_to_ecef(const GCSPoint& _point);
  GCSPoint ecef_to_gcs(const ECEFPoint& _point);
}

#endif
