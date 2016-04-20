#pragma once

namespace CSUtils
{
  /**
   * @enum  CSType
   *
   * @brief Values that represent coordinate system types.
   */

  enum CSType
  {
    ///< Spherical Coordinate System
    SPH_CST,
    ///< Local Cartesian Coordinate System (East North UP - ENU)
    ENU_CST,
    ///< Cartesian Coordinate System of Antenna
    CCSA_CST,
    ///< Biconical Coordinate System
    BCS_CST,
    ///< Geodetic Coordiane System
    GCS_CST,
    ///< Geocentric Coordinate System (Earth Center Earth Fixed - ECEF)
    ECEF_CST,
    ///< An enum constant representing the unknown coordinate system
    UNKNOWN_CST
  };

  /*
      NOTE: in points all linear values in meters and all angular values in radians
  */

  /**
   * @struct  ENUPoint
   *
   * @brief The point in East North Up coordinate system.
   *
   * @author  Gorgool
   * @date  20.04.2016
   */

  struct ENUPoint
  {
    double x, y, z;
    ENUPoint(double _x = 0.0, double _y = 0.0, double _z = 0.0) : x(_x), y(_y), z(_z) {};
  };

  /**
   * @struct  CCSAPoint
   *
   * @brief The point in Cartesian Coordiante System of Antenna.
   *
   * @author  Gorgool
   * @date  20.04.2016
   */

  struct CCSAPoint
  {
    double x, y, z;
    CCSAPoint(double _x = 0.0, double _y = 0.0, double _z = 0.0) : x(_x), y(_y), z(_z) {};
  };

  /**
   * @struct  BCSPoint
   *
   * @brief The point in Biconical coordiante system.
   *
   * @author  Gorgool
   * @date  20.04.2016
   */

  struct BCSPoint
  {
    double R, beta, epsilon;
    BCSPoint(double _R = 0.0, double _beta = 0.0, double _epsilon = 0.0) : R(_R), beta(_beta), epsilon(_epsilon) {};
  };

  /**
   * @struct  ECEFPoint
   *
   * @brief The point in Earth Center Earth Fixed coordinate system.
   *
   * @author  Gorgool
   * @date  20.04.2016
   */

  struct ECEFPoint
  {
    double x, y, z;
    ECEFPoint(double _x = 0.0, double _y = 0.0, double _z = 0.0) : x(_x), y(_y), z(_z) {};
  };

  /**
   * @struct  GCSPoint
   *
   * @brief The point in Geodetic coordiante system.
   *
   * @author  Gorgool
   * @date  20.04.2016
   */

  struct GCSPoint
  {
    double latitude, longitude,  altitude;
    GCSPoint(
      double _latitude = 0.0,
      double _longitude = 0.0,
      double _altitude = 0.0) : 
    latitude(_latitude), longitude(_longitude), altitude(_altitude) {};
  };

  /**
   * @struct  SPHPoint
   *
   * @brief A point in Spherical coordinate system.
   *
   * @author  Gorgool
   * @date  20.04.2016
   */

  struct SPHPoint
  {
    double R, theta, phi;
    SPHPoint(double _R = 0.0, double _theta = 0.0, double _phi = 0.0) : R(_R), theta(_theta), phi(_phi) {};
  };
}
