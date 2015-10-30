#pragma once

namespace CSUtils
{
  /*
  Coordinate system types
  */
  enum CSType
  {
    ENU_CST,    // Local Cartesian Coordinate System (ENU)
    CCSA_CST,   // Cartesian Coordinate System of Antenna
    BCS_CST,    // Biconical Coordinate System
    GCS_CST,    // Geodetic Coordiane System
    ECEF_CST,   // Geocentric Coordinate System (ECEF)
    UNKNOWN_CST
  };

  enum UnitType 
  { 
    Tx_UT,     // Transmiter
    Rx_UT,     // Reciever
    None_UT 
  };

  // Abstract 3-parameters point
  struct AbstractPoint
  {
  	double a, b, c;
    CSType type;
    UnitType host;
    AbstractPoint(double _a, double _b, double _c, CSType _type, UnitType _host) : a(_a), b(_b), c(_c), type(_type), host(_host) {}
  };

  // Interface to convert any point to abstract
  class IAbstractPoint
  {
  public:
    virtual AbstractPoint to_abstract() const = 0;
    virtual void from_abstruct(const AbstractPoint& _point) = 0;
  };

  /*
      NOTE: in points all linear values in meters and all angular values in radians
  */

  struct ENUPoint : public IAbstractPoint
  {
    CSType type;
    double x, y, z;
    UnitType host;
    ENUPoint(double _x = 0.0, double _y = 0.0, double _z = 0.0, UnitType _h = Tx_UT) : type(ENU_CST), x(_x), y(_y), z(_z), host(_h) {};
    ENUPoint(const AbstractPoint& _point) : type(ENU_CST), x(_point.a), y(_point.b), z(_point.c), host(_point.host) {};

    AbstractPoint to_abstract() const
    {
      return AbstractPoint(x, y, z, type, host);
    }

    void from_abstruct(const AbstractPoint& _point)
    {
      x = _point.a; y = _point.b; z = _point.c; host = _point.host;
    }
  };

  struct CCSAPoint : public IAbstractPoint
  {
    CSType type;
    double x, y, z;
    UnitType host;
    CCSAPoint(double _x = 0.0, double _y = 0.0, double _z = 0.0, UnitType _h = Tx_UT) : type(CCSA_CST), x(_x), y(_y), z(_z), host(_h) {};
    CCSAPoint(const AbstractPoint& _point) : type(CCSA_CST), x(_point.a), y(_point.b), z(_point.c), host(_point.host) {};

    AbstractPoint to_abstract() const
    {
      return AbstractPoint(x, y, z, type, host);
    }

    void from_abstruct(const AbstractPoint& _point)
    {
      x = _point.a; y = _point.b; z = _point.c; host = _point.host;
    }
  };

  struct BCSPoint : public IAbstractPoint
  {
    CSType type;
    double R, beta, epsilon;
    UnitType host; 
    BCSPoint(double _R = 0.0, double _beta = 0.0, double _epsilon = 0.0, UnitType _h = Tx_UT) : type(BCS_CST), R(_R), beta(_beta), epsilon(_epsilon), host(_h) {};
    BCSPoint(const AbstractPoint& _point) : type(BCS_CST), R(_point.a), beta(_point.b), epsilon(_point.c), host(_point.host) {};

    AbstractPoint to_abstract() const
    {
      return AbstractPoint(R, beta, epsilon, type, host);
    }

    void from_abstruct(const AbstractPoint& _point)
    {
      R = _point.a; beta = _point.b; epsilon = _point.c; host = _point.host;
    }
  };

  struct ECEFPoint : public IAbstractPoint
  {
    CSType type;
    double x, y, z;
    UnitType host;
    ECEFPoint(double _x = 0.0, double _y = 0.0, double _z = 0.0) : type(ECEF_CST), x(_x), y(_y), z(_z), host(None_UT) {};
    ECEFPoint(const AbstractPoint& _point) : type(ECEF_CST), x(_point.a), y(_point.b), z(_point.c), host(None_UT) {};

    AbstractPoint to_abstract() const
    {
      return AbstractPoint(x, y, z, type, None_UT);
    }

    void from_abstruct(const AbstractPoint& _point)
    {
      x = _point.a; y = _point.b; z = _point.c; host = None_UT;
    }
  };

  struct GCSPoint : public IAbstractPoint
  {
    CSType type;
    double latitude, longitude,  altitude;
    UnitType host;
    GCSPoint(
      double _latitude = 0.0,
      double _longitude = 0.0,
      double _altitude = 0.0) : 
    type(GCS_CST), latitude(_latitude), longitude(_longitude), altitude(_altitude), host(None_UT) {};
    GCSPoint(const AbstractPoint& _point) : 
      type(GCS_CST), latitude(_point.a), longitude(_point.b), altitude(_point.c), host(None_UT) {};

    AbstractPoint to_abstract() const
    {
      return AbstractPoint(latitude, longitude, altitude, type, None_UT);
    }

    void from_abstruct(const AbstractPoint& _point)
    {
      latitude = _point.a; longitude = _point.b; altitude = _point.c; host = None_UT;
    }
  };
}
