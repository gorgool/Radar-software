#pragma once

namespace CSUtils
{
  /*
  Coordinate system types
  */
  enum CSType
  {
    SPH_CST,    // Spherical Coordinate System
    ENU_CST,    // Local Cartesian Coordinate System (ENU)
    CCSA_CST,   // Cartesian Coordinate System of Antenna
    BCS_CST,    // Biconical Coordinate System
    GCS_CST,    // Geodetic Coordiane System
    ECEF_CST,   // Geocentric Coordinate System (ECEF)
    UNKNOWN_CST
  };

  /*
      NOTE: in points all linear values in meters and all angular values in radians
  */

  struct ENUPoint
  {
    double x, y, z;
    ENUPoint(double _x = 0.0, double _y = 0.0, double _z = 0.0) : x(_x), y(_y), z(_z) {};
  };

  struct CCSAPoint
  {
    double x, y, z;
    CCSAPoint(double _x = 0.0, double _y = 0.0, double _z = 0.0) : x(_x), y(_y), z(_z) {};
  };

  struct BCSPoint
  {
    double R, beta, epsilon;
    BCSPoint(double _R = 0.0, double _beta = 0.0, double _epsilon = 0.0) : R(_R), beta(_beta), epsilon(_epsilon) {};
  };

  struct ECEFPoint
  {
    double x, y, z;
    ECEFPoint(double _x = 0.0, double _y = 0.0, double _z = 0.0) : x(_x), y(_y), z(_z) {};
  };

  struct GCSPoint
  {
    double latitude, longitude,  altitude;
    GCSPoint(
      double _latitude = 0.0,
      double _longitude = 0.0,
      double _altitude = 0.0) : 
    latitude(_latitude), longitude(_longitude), altitude(_altitude) {};
  };

  struct SPHPoint
  {
    double R, theta, phi;
    SPHPoint(double _R = 0.0, double _theta = 0.0, double _phi = 0.0) : R(_R), theta(_theta), phi(_phi) {};
  };
}
