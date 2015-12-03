#pragma once

#include <string>
#include <sstream>
#include <chrono>
#include <array>
#include "../../Shared/include/ReferencePointDesc.h"

namespace TargetEnvironment
{
  using ClockType = std::chrono::system_clock;
  using TimeType = ClockType::time_point;
  /*
    From client to server
  */

  // Referance point. Describes geodetic coordinates and sector parameters.
  struct RefPoint
  {
    std::string msg;
    RefPoint(const ReferencePointDesc& rfp_desc)
    {
      std::stringstream s;
      s << "{ \"message\" : \"RefPoint\""
        << ", \"id\" : " << rfp_desc.client_id
        << ", \"lat\" : " << rfp_desc.latitude
        << ", \"lon\" : " << rfp_desc.longitude
        << ", \"alt\" : " << rfp_desc.height
        << ", \"range\" : " << rfp_desc.range
        << ", \"elevation_angle\" : " << rfp_desc.elevation_ang
        << ", \"elevation_width\" : " << rfp_desc.elevation_width
        << ", \"azimuth_angle\" : " << rfp_desc.azimuth_ang
        << ", \"azimuth_width\" : " << rfp_desc.azimuth_width
        << "}\n";



      /*s << "RefPoint [ id:" << rfp_desc.client_id
        << "; lat:" << rfp_desc.latitude
        << "; lon:" << rfp_desc.longitude
        << "; alt:" << rfp_desc.height
        << "; range:" << rfp_desc.range
        << "; elevation_angle:" << rfp_desc.elevation_ang
        << "; elevation_width:" << rfp_desc.elevation_width
        << "; azimuth_angle:" << rfp_desc.azimuth_ang
        << "; azimuth_width:" << rfp_desc.azimuth_width
        << ";]\n";*/
      msg = s.str();
    }
  };

  // Request targets message
  struct Request
  {
    std::string msg;
    //time
    Request(const std::size_t id, const ClockType::duration::rep& time)
    {
      std::stringstream s;
      s << "{ \"message\" : \"Request\""
        << ", \"id\" : " << id
        << ", \"time\" : " << time
        << "}\n";
      /*s << "Request [ id:" << id
        << "; time:" << time
        << ";]\n";*/
      msg = s.str();
    }
  };

  /*
    From server to client
  */

  // Reference point register fail message
  const std::string RefPointFail = "RefPointFail\n";
  // Register Success message
  const std::string RegisterSuccess = "RegisterOK\n";
  // If request cant be processd message
  const std::string RequestFail = "RequestFail\n";
  // Request reply message
  struct TargetsList
  {
    std::string msg;
    TargetsList(const std::size_t count)
    {
      std::stringstream s;
      s << " { \"message\" : \"TargetsList\""
        << ", \"size\" : " << count
        << "}\n";
      /*s << "TargetsList [ size:" << count
        << ";]\n";*/
      msg = s.str();
    }
  };
  // Description of target
#pragma pack(push, 1)
  struct TargetDesc
  {
    std::size_t target_id;

    // X coordinate vector, meters, metera/sec, meters/(sec*sec), etc.
    std::array<double, 6> x;

    // Y coordinate vector, meters, metera/sec, meters/(sec*sec), etc.
    std::array<double, 6> y;

    // Z coordinate vector, meters, metera/sec, meters/(sec*sec), etc.
    std::array<double, 6> z;

    TargetDesc(std::size_t _id, 
               const std::array<double, 6>& x_params,
               const std::array<double, 6>& y_params, 
               const std::array<double, 6>& z_params) :
      target_id(_id),
      x(x_params),
      y(y_params),
      z(z_params) {}

    TargetDesc() = default;
  };
}
#pragma pack(pop)