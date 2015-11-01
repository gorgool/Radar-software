#pragma once

#include <string>
#include <sstream>
#include <chrono>
#include <array>

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
    RefPoint(const std::size_t id, double lat, const double lon, const double alt, const double range)
    {
      std::stringstream s;
      s << "RefPoint [ id:" << id
        << "; lat:" << lat
        << "; lon:" << lon
        << "; alt:" << alt
        << "; range:" << range
        << ";]\n";
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
      s << "Request [ id:" << id
        << "; time:" << time
        << ";]\n";
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
      s << "TargetsList [ size:" << count
        << ";]\n";
      msg = s.str();
    }
  };
  // Description of target
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
               std::array<double, 6>& x_params,
               std::array<double, 6>& y_params, 
               std::array<double, 6>& z_params) :
      target_id(_id),
      x(x_params),
      y(y_params),
      z(z_params) {}

    TargetDesc() = default;
  };
}