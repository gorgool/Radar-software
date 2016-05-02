#pragma once

#include <string>

namespace MessageBroker
{
  /**
   * @fn  std::string make_heartbeat_message(std::string id)
   *
   * @brief Makes heartbeat message.
   *
   * @author  Gorgool
   * @date  26-Mar-16
   *
   * @param id  The identifier.
   *
   * @return  A std::string.
   */

  std::string make_heartbeat_message(std::string id)
  {
    return R"({ "message" : "hearteat", "from" : ")" + id + R"(" })" "\n";
  }
}