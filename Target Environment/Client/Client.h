#pragma once

// Standart headers
#include <chrono>
#include <unordered_map>

// Boost
#include <boost/asio.hpp>

// RapidJSON header
#include <rapidjson/document.h>

// Internal module headers
#include "Shared/SearchArea.h"
#include "Shared/Errors.h"
#include "Shared/Messages.h"
#include "Shared/TargetTable.h"
#include "CoordinateSystems/CSUtils.h"
#include "Config Manager/ConfigManager.hpp"

namespace TargetEnvironment
{
  class Client
  {
    /** @brief The boost asio service. */
    boost::asio::io_service _service;
    /** @brief The socket. */
    boost::asio::ip::tcp::socket _socket;

    /** @brief The server endpoint. */
    boost::asio::ip::tcp::endpoint _server_endpoint;

    /** @brief true if connected. */
    bool _connected;

    /** @brief true if configuration loaded. */
    bool _config_loaded;

    /**
     * @fn  ErrorCode Client::deserialize(const std::string& msg, TargetTable& tbl);
     *
     * @brief deserialize json string message to the goven table.
     *
     * @author  Gorgool
     * @date  20.04.2016
     *
     * @param msg           The json message string.
     * @param [out] tbl     The table.
     *
     * @return  An ErrorCode.
     */

    ErrorCode deserialize(const std::string& msg, TargetTable& tbl);

    /**
     * @fn  ErrorCode Client::close_client();
     *
     * @brief Closes and cleanup the client.
     *
     * @author  Gorgool
     * @date  20.04.2016
     *
     * @return  An ErrorCode.
     */

    ErrorCode close_client();

  public:
    Client();

    ~Client();

    /**
     * @fn  ErrorCode Client::load_config(const ConfigManager& conf);
     *
     * @brief Loads a configuration.
     *
     * @author  Gorgool
     * @date  20.04.2016
     *
     * @param conf  The configuration manager object.
     *
     * @return  OK if read and parsing was success. Return SystemError if file was not found and ConfigError due to parse error..
     */

    ErrorCode load_config(const ConfigManager& conf);

    /**
     * @fn  ErrorCode Client::connect();
     *
     * @brief Connects to servert.
     *
     * @author  Gorgool
     * @date  20.04.2016
     *
     * @return  OK if succeeded, RegisterFail or SystemError otherwise.
     */

    ErrorCode connect();

    /**
     * @fn  ErrorCode Client::disconnect();
     *
     * @brief Disconnect from server.
     *
     * @author  Gorgool
     * @date  20.04.2016
     *
     * @return  OK if succeeded, SystemError otherwise.
     */

    ErrorCode disconnect();

    /**
     * @fn  ErrorCode Client::get_targets(TargetEnvironment::TargetTable& tbl, const TargetEnvironment::SearchArea& area, const CSUtils::GCSPoint& radar_loc, const ClockType::time_point& time);
     *
     * @brief Request targets list (by time t) from server.
     *
     * @author  Gorgool
     * @date  20.04.2016
     *
     * @param [out]  tbl    The targets list.
     * @param area          The search area descriptor.
     * @param radar_loc     The radar location.
     * @param time          The time.
     *
     * @return  OK if request procced succefully, RequestFail of SystemError otherwise.
     */

    ErrorCode get_targets(TargetEnvironment::TargetTable& tbl, const TargetEnvironment::SearchArea& area, const CSUtils::GCSPoint& radar_loc, const ClockType::time_point& time);
  };
}
