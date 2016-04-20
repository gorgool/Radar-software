#include "CSTransform.h"

/**
 * @fn  double CSUtils::degree_to_rad(double _degrees, double _minutes, double _seconds)
 *
 * @brief Convert degrees to radians.
 *
 * @author  Gorgool
 * @date  20.04.2016
 *
 * @param _degrees  The angular degrees.
 * @param _minutes  The angular minutes.
 * @param _seconds  The angular seconds.
 *
 * @return  Angle in radians.
 */

double CSUtils::degree_to_rad(double _degrees, double _minutes, double _seconds)
{
  return M_PI * (_degrees + (_minutes + _seconds / 60.0) / 60.0) / 180.0;
};

/**
 * @fn  double CSUtils::degree_to_rad(const CSUtils::Angle& _angle)
 *
 * @brief Converts degree to radians.
 *
 * @author  Gorgool
 * @date  20.04.2016
 *
 * @param _angle  The angle.
 *
 * @return  Angle in radians.
 */

double CSUtils::degree_to_rad(const CSUtils::Angle& _angle)
{
  return M_PI * (_angle.degrees + (_angle.minutes + _angle.seconds / 60.0) / 60.0) / 180.0;
};

/**
 * @fn  CSUtils::Angle CSUtils::rad_to_degrees(double _rads)
 *
 * @brief Converts radians to degrees.
 *
 * @author  Gorgool
 * @date  20.04.2016
 *
 * @param _rads Angle in radians.
 *
 * @return  The Angle.
 */

CSUtils::Angle CSUtils::rad_to_degrees(double _rads)
{
  double temp = _rads * 180.0 / M_PI;
  unsigned int degrees = static_cast<unsigned int>(temp);
  unsigned int minutes = static_cast<unsigned int>((temp - static_cast<double>(degrees)) * 60.0);
  double seconds = ((temp - static_cast<double>(degrees)) * 60.0 - static_cast<double>(minutes)) * 60.0;

  return CSUtils::Angle(degrees, minutes, seconds);
};

/**
 * @fn  CSUtils::ECEFPoint CSUtils::gcs_to_ecef(const CSUtils::GCSPoint& _point)
 *
 * @brief Converts point from Geodetic coordinate system to Earth Center Earth Fixed coordinate system.
 *
 * @author  Gorgool
 * @date  20.04.2016
 *
 * @param _point  The point in Geodetic coordinate system.
 *
 * @return  Earth Center Earth Fixed coordinate system point.
 */

CSUtils::ECEFPoint CSUtils::gcs_to_ecef(const CSUtils::GCSPoint& _point)
{
  CSUtils::ECEFPoint ret;

  double temp = CSUtils::a / sqrt(1.0 - CSUtils::e_2 * sin(_point.latitude) * sin(_point.latitude));

  ret.x = (temp + _point.altitude) * cos(_point.latitude) * cos(_point.longitude);
  ret.y = (temp + _point.altitude) * cos(_point.latitude) * sin(_point.longitude);
  ret.z = (temp * (1.0 - CSUtils::e_2) + _point.altitude) * sin(_point.latitude);

  return ret;
}

/**
 * @fn  CSUtils::GCSPoint CSUtils::ecef_to_gcs(const CSUtils::ECEFPoint& _point)
 *
 * @brief Converts point from Earth Center Earth Fixed coordinate system to Geodetic coordinate system.
 *
 * @author  Gorgool
 * @date  20.04.2016
 *
 * @param _point  Earth Center Earth Fixed coordinate system point.
 *
 * @return  The point in Geodetic coordinate system.
 */

CSUtils::GCSPoint CSUtils::ecef_to_gcs(const CSUtils::ECEFPoint& _point)
{
  double p = sqrt(_point.x * _point.x + _point.y * _point.y);
  double tan_phi = _point.z / p;

  // run 4 iterations
  for (size_t times = 0; times < 4; times++)
  {
    tan_phi = (_point.z + CSUtils::a * CSUtils::e_2 * tan_phi / sqrt(1.0 + (1.0 - CSUtils::e_2) * tan_phi * tan_phi) ) / p;
  }

  CSUtils::GCSPoint ret;

  ret.latitude = atan(tan_phi);
  ret.longitude = atan2(_point.y, _point.x);

  double temp = CSUtils::a / sqrt(1.0 + (1.0 - CSUtils::e_2) * tan_phi * tan_phi);

  ret.altitude = sqrt(pow(p - temp, 2) + pow(_point.z - temp * (1.0 - CSUtils::e_2) * tan_phi, 2));

  return ret;
}

/**
 * @fn  CSUtils::ENUPoint CSUtils::ecef_to_enu(const CSUtils::ECEFPoint& _point, const CSUtils::GCSPoint& _origin)
 *
 * @brief Converts point from Earth Center Earth Fixed coordinate system to East North Up coordinate system with specific origin point.
 *
 * @author  Gorgool
 * @date  20.04.2016
 *
 * @param _point  The point in Earth Center Earth Fixed coordinate system.
 * @param _origin The origin of East North Up coordinate system.
 *
 * @return  East North Up coordinate system point.
 */

CSUtils::ENUPoint CSUtils::ecef_to_enu(const CSUtils::ECEFPoint& _point, const CSUtils::GCSPoint& _origin)
{
  double phi = _origin.latitude;
  double lamda = _origin.longitude;
  double h = _origin.altitude;

  Eigen::Vector3d lccs_center_point;

  lccs_center_point(0) = (a * cos(atan(b * tan(phi) / a)) + h * cos(phi)) * cos(lamda);
  lccs_center_point(1) = (a * cos(atan(b * tan(phi) / a)) + h * cos(phi)) * sin(lamda);
  lccs_center_point(2) = b * sin(atan(b * tan(phi) / a)) + h * sin(phi);

  Eigen::Matrix3d C_1 = Eigen::Matrix3d::Zero();
  C_1(0, 2) = -1.0; C_1(1, 1) = 1.0; C_1(2, 0) = 1.0;

  Eigen::Matrix3d C_2 = Eigen::Matrix3d::Zero();
  C_2(0, 0) = 1.0;
  C_2(1, 1) = cos(phi);   C_2(1, 2) = -sin(phi);
  C_2(2, 1) = sin(phi);   C_2(2, 2) = cos(phi);

  Eigen::Matrix3d C_3 = Eigen::Matrix3d::Zero();
  C_3(0, 0) = sin(lamda);    C_3(0, 1) = cos(lamda);   
  C_3(1, 0) = -cos(lamda);   C_3(1, 1) = sin(lamda);
  C_3(2, 2) = 1.0;

  Eigen::Matrix3d C = C_3 * C_2 * C_1;
  C.transposeInPlace();
  Eigen::Vector3d result = C * Eigen::Vector3d(_point.x, _point.y, _point.z) - C * lccs_center_point;

  return CSUtils::ENUPoint(result(0), result(1), result(2));
}

/**
 * @fn  CSUtils::ECEFPoint CSUtils::enu_to_ecef(const CSUtils::ENUPoint& _point, const CSUtils::GCSPoint& _origin)
 *
 * @brief Converts point from East North Up coordinate system to Earth Center Earth Fixed coordinate system with specific origin point.
 *
 * @author  Gorgool
 * @date  20.04.2016
 *
 * @param _point  The point in East North Up coordinate system.
 * @param _origin The origin of East North Up coordinate system.
 *
 * @return  The point in Earth Center Earth Fixed coordinate system.
 */

CSUtils::ECEFPoint CSUtils::enu_to_ecef(const CSUtils::ENUPoint& _point, const CSUtils::GCSPoint& _origin)
{
  double phi = _origin.latitude;
  double lamda = _origin.longitude;
  double h = _origin.altitude;

  Eigen::Vector3d lccs_center_point;

  lccs_center_point(0) = (a * cos(atan(b * tan(phi) / a)) + h * cos(phi)) * cos(lamda);
  lccs_center_point(1) = (a * cos(atan(b * tan(phi) / a)) + h * cos(phi)) * sin(lamda);
  lccs_center_point(2) = b * sin(atan(b * tan(phi) / a)) + h * sin(phi);

  Eigen::Matrix3d C_1 = Eigen::Matrix3d::Zero();
  C_1(0, 2) = -1.0; C_1(1, 1) = 1.0; C_1(2, 0) = 1.0;

  Eigen::Matrix3d C_2 = Eigen::Matrix3d::Zero();
  C_2(0, 0) = 1.0;
  C_2(1, 1) = cos(phi);   C_2(1, 2) = -sin(phi);
  C_2(2, 1) = sin(phi);   C_2(2, 2) = cos(phi);

  Eigen::Matrix3d C_3 = Eigen::Matrix3d::Zero();
  C_3(0, 0) = sin(lamda);    C_3(0, 1) = cos(lamda);   
  C_3(1, 0) = -cos(lamda);   C_3(1, 1) = sin(lamda);
  C_3(2, 2) = 1.0;

  Eigen::Matrix3d C = C_3 * C_2 * C_1;

  Eigen::Vector3d ret = lccs_center_point + C * Eigen::Vector3d(_point.x, _point.y, _point.z);

  return CSUtils::ECEFPoint(ret(0), ret(1), ret(2));
}

/**
 * @fn  CSUtils::CCSAPoint CSUtils::enu_to_ccsa(const ENUPoint& _point, const Angle& _eps, const Angle& _beta)
 *
 * @brief Converts point from East North Up coordinate system to Cartesian Coordinate System of Antenna with specific orientation.
 *
 * @author  Gorgool
 * @date  20.04.2016
 *
 * @param _point  The point in East North Up coordinate system.
 * @param _eps    The elevation angle of antenna orientation.
 * @param _beta   The azimuth angle of antenna orientation.
 *
 * @return  Cartesian Coordinate System of Antenna point.
 */

CSUtils::CCSAPoint CSUtils::enu_to_ccsa(const ENUPoint& _point, const Angle& _eps, const Angle& _beta)
{
  double eps = degree_to_rad(_eps);
  double beta = degree_to_rad(_beta);

  Eigen::Matrix3d C_1 = Eigen::Matrix3d::Zero();
  C_1(0, 0) = cos(beta); C_1(0, 2) = -sin(beta);
  C_1(1, 1) = 1.0;
  C_1(2, 0) = sin(beta); C_1(2, 2) = cos(beta);

  Eigen::Matrix3d C_2 = Eigen::Matrix3d::Zero();
  C_2(0, 0) = cos(eps); C_2(0, 1) = sin(eps);
  C_2(1, 0) = -sin(eps); C_2(1, 1) = cos(eps);
  C_2(2, 2) = 1.0;

  Eigen::Vector3d ret = C_2 * C_1 * Eigen::Vector3d(_point.x, _point.y, _point.z);

  return CSUtils::CCSAPoint(ret(0), ret(1), ret(2));
}

/**
 * @fn  CSUtils::ENUPoint CSUtils::ccsa_to_enu(const CCSAPoint& _point, const Angle& _eps, const Angle& _beta)
 *
 * @brief Converts point from Cartesian Coordinate System of Antenna to East North Up coordinate system with specific orientation.
 *
 * @author  Gorgool
 * @date  20.04.2016
 *
 * @param _point  The point in Cartesian Coordinate System of Antenna.
 * @param _eps    The elevation angle of antenna orientation.
 * @param _beta   The azimuth angle of antenna orientation.
 *
 * @return  East North Up coordinate system point.
 */

CSUtils::ENUPoint CSUtils::ccsa_to_enu(const CCSAPoint& _point, const Angle& _eps, const Angle& _beta)
{
  double eps = degree_to_rad(_eps);
  double beta = degree_to_rad(_beta);

  Eigen::Matrix3d C_1 = Eigen::Matrix3d::Zero();
  C_1(0, 0) = cos(beta); C_1(0, 2) = -sin(beta);
  C_1(1, 1) = 1.0;
  C_1(2, 0) = sin(beta); C_1(2, 2) = cos(beta);

  Eigen::Matrix3d C_2 = Eigen::Matrix3d::Zero();
  C_2(0, 0) = cos(eps); C_2(0, 1) = sin(eps);
  C_2(1, 0) = -sin(eps); C_2(1, 1) = cos(eps);
  C_2(2, 2) = 1.0;

  Eigen::Vector3d ret = (C_2 * C_1).transpose() * Eigen::Vector3d(_point.x, _point.y, _point.z);

  return CSUtils::ENUPoint(ret(0), ret(1), ret(2));
}

/**
 * @fn  CSUtils::BCSPoint CSUtils::ccsa_to_bcs(const CSUtils::CCSAPoint& _point)
 *
 * @brief Converts point from Cartesian Coordinate System of Antenna to Biconical coordinate system.
 *
 * @author  Gorgool
 * @date  20.04.2016
 *
 * @param _point  The point in Cartesian Coordinate System of Antenna.
 *
 * @return  Biconical coordinate system point.
 */

CSUtils::BCSPoint CSUtils::ccsa_to_bcs(const CSUtils::CCSAPoint& _point)
{
  CSUtils::BCSPoint ret;
  ret.R = sqrt(_point.x * _point.x + _point.y * _point.y + _point.z * _point.z);
  if ( ret.R <= std::numeric_limits<double>::epsilon())
  {
    ret.epsilon = 0.0;
    ret.beta = 0.0;
  }
  else
  {
    ret.epsilon = _point.y / ret.R;
    ret.beta = - _point.z / ret.R;
  }
  return ret;
}

/**
 * @fn  CSUtils::CCSAPoint CSUtils::bcs_to_ccsa(const CSUtils::BCSPoint& _point)
 *
 * @brief Converts point from Biconical coordinate system to Cartesian Coordinate System of Antenna.
 *
 * @author  Gorgool
 * @date  20.04.2016
 *
 * @param _point  The point in Biconical coordinate system.
 *
 * @return  Cartesian Coordinate System of Antenna point.
 */

CSUtils::CCSAPoint CSUtils::bcs_to_ccsa(const CSUtils::BCSPoint& _point)
{
  CSUtils::CCSAPoint ret;
  ret.x = _point.R * sqrt(1 - _point.epsilon * _point.epsilon - _point.beta * _point.beta);
  ret.y = _point.R * _point.epsilon;
  ret.z = - _point.R * _point.beta;

  return ret;
}

/**
 * @fn  CSUtils::ENUPoint CSUtils::sph_to_enu(const CSUtils::SPHPoint& _point)
 *
 * @brief Converts point from Spherical coordinate system to East North Up coordinate system.
 *
 * @author  Gorgool
 * @date  20.04.2016
 *
 * @param _point  The point in Spherical coordinate system.
 *
 * @return  East North Up coordinate system point.
 */

CSUtils::ENUPoint CSUtils::sph_to_enu(const CSUtils::SPHPoint& _point)
{
  CSUtils::ENUPoint ret;

  ret.x = _point.R * sin(_point.theta) * cos(_point.phi);
  ret.y = _point.R * sin(_point.theta) * sin(_point.phi);
  ret.z = _point.R * cos(_point.theta);

  return ret;
}

/**
 * @fn  CSUtils::SPHPoint CSUtils::enu_to_sph(const CSUtils::ENUPoint& _point)
 *
 * @brief Converts point from East North Up coordinate system to Spherical coordinate system.
 *
 * @author  Gorgool
 * @date  20.04.2016
 *
 * @param _point  The point in East North Up coordinate system.
 *
 * @return  Spherical coordinate system point.
 */

CSUtils::SPHPoint CSUtils::enu_to_sph(const CSUtils::ENUPoint& _point)
{
  CSUtils::SPHPoint ret;

  ret.R = sqrt(_point.x * _point.x + _point.y * _point.y + _point.z * _point.z);
  ret.theta = atan2(sqrt(_point.x * _point.x + _point.y * _point.y), _point.z);
  ret.phi = atan2(_point.y, _point.z);

  return ret;
}