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

namespace TargetEnvironment
{
  class Client
  {
    boost::asio::io_service _service;
    boost::asio::ip::tcp::socket _socket;

    // Desealization function helper
    ErrorCode deserialize(const std::string&, TargetTable& tbl);

    // Session cleanup helper
    ErrorCode close_client();

    // Server endpoint
    boost::asio::ip::tcp::endpoint _server_endpoint;

    // Connection to server flag
    bool _connected;

    // Config loaded flag
    bool _config_loaded;

  public:
    Client();

    ~Client();

    // Read and parse configuration file. Return OK if read and parsing was success. 
    // Return SystemError if file was not found and ConfigError due to parse error.
    ErrorCode load_config(std::string path_to_file);

    // Connect to server. Return OK if succeeded, RegisterFail or SystemError otherwise.
    ErrorCode connect();

    // Disconnect from server. Return OK if succeeded, SystemError otherwise.
    ErrorCode disconnect();

    // Request targets list (by time t) from server. Return OK if request procced succefully, RequestFail of SystemError otherwise.
    ErrorCode get_targets(TargetEnvironment::TargetTable&, const TargetEnvironment::SearchArea&, const CSUtils::GCSPoint&, const ClockType::time_point& time);
  };
}
