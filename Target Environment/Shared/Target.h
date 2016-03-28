#pragma once

#include <cinttypes>
#include <array>

namespace TargetEnvironment
{
  // Description of target
  struct Target
  {
    std::uint32_t target_id;

    // X coordinate vector, meters, metera/sec, meters/(sec*sec), etc.
    std::array<double, 6> x;

    // Y coordinate vector, meters, metera/sec, meters/(sec*sec), etc.
    std::array<double, 6> y;

    // Z coordinate vector, meters, metera/sec, meters/(sec*sec), etc.
    std::array<double, 6> z;

    Target(std::uint32_t _id,
      const std::array<double, 6>& x_params,
      const std::array<double, 6>& y_params,
      const std::array<double, 6>& z_params) :
      target_id(_id),
      x(x_params),
      y(y_params),
      z(z_params) {}

    Target() = default;

    bool operator==(const Target& obj)
    {
      return target_id == obj.target_id;
    }

    bool operator!=(const Target& obj)
    {
      return !(*this == obj);
    }
  };
}