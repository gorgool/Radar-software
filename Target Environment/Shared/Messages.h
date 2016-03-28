#pragma once

#include <string>
#include <sstream>
#include <chrono>
#include <array>
#include "Shared/Target.h"
#include "Utils/FormatString.h"
#include "CoordinateSystems/CSUtils.h"

namespace TargetEnvironment
{
  using ClockType = std::chrono::system_clock;
  using DurationType = std::chrono::duration<uint64_t, std::ratio_multiply<std::ratio<100, 1>, std::nano>>;
  using TimeType = std::chrono::time_point<ClockType, DurationType>;
  
  /*
    From client to server
  */

  // Request targets message
  struct RequestMsg
  {
    std::string msg;

    RequestMsg(const SearchArea& search_area, const CSUtils::GCSPoint& radar_location, const ClockType::duration::rep& time)
    {
      setlocale(LC_ALL, "en");
      msg = Utils::string_format(
        R"({ "message" : "TE_Request", "latitude" : %.8f, "longitude" : %.8f, "altitude" : %.8f,)"
        R"("type" : "spherical_sector", "X" : %.8f , "Y" : %.8f, "Z" : %.8f,  "range" : %.8f,)"
        R"("elevation_angle" : %.8f, "elevation_width" : %.8f,)"
        R"("azimuth_angle" : %.8f, "azimuth_width" : %.8f, "time" : %)" PRIu64 "}\n",
        radar_location.latitude,
        radar_location.longitude,
        radar_location.altitude,
        search_area.x,
        search_area.y,
        search_area.z,
        search_area.range,
        search_area.elevation_angle,
        search_area.elevation_width,
        search_area.azimuth_angle,
        search_area.azimuth_width,
        time);
    }
  };

  /*
    From server to client
  */

  // Register fail message
  const std::string RegisterFailMsg = R"({ "message" : "TE_RegisterFail" })" "\n";
  // Register Success message
  const std::string RegisterSuccessMsg = R"({ "message" : "TE_RegisterSuccess" })" "\n";
  // If request cant be processd message
  const std::string RequestFailMsg = R"({ "message" : "TE_RequestFail" })" "\n";

  // Targets List message
  struct TargetsListMsg
  {
    std::string msg;

    TargetsListMsg(std::vector<TargetEnvironment::Target>& targets)
    {
      setlocale(LC_ALL, "en");

      msg = Utils::string_format(R"({ "message" : "TE_TargetsList", "size" : %u, "targets" : [)", targets.size());

      auto last_item = targets.back();

      for (auto& item : targets)
      {
        std::string item_buff = Utils::string_format(
          R"({ "id" : %u,)" 
          R"("X" : [%.8f, %.8f, %.8f, %.8f, %.8f, %.8f],)" 
          R"("Y" : [%.8f, %.8f, %.8f, %.8f, %.8f, %.8f],)"
          R"("Z" : [%.8f, %.8f, %.8f, %.8f, %.8f, %.8f] })",
          item.target_id,
          item.x[0], item.x[1], item.x[2], item.x[3], item.x[4], item.x[5],
          item.y[0], item.y[1], item.y[2], item.y[3], item.y[4], item.y[5],
          item.z[0], item.z[1], item.z[2], item.z[3], item.z[4], item.z[5]
          );

        if (item != last_item)
        {
          item_buff += ", ";
        }

        msg += item_buff;
      }
      msg += "]}\n";
    }

    TargetsListMsg() = default;
  };
}
