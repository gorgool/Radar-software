#pragma once

// Standard headers
#include <ctime>
#include <inttypes.h>
#include <cstddef>
#include <list>
#include <mutex>

// Boost headers
#include <boost/asio.hpp>
#include <boost/thread.hpp>

// RapidJSON header
#include <rapidjson/document.h>

// Eigen header wrapper
#include "Utils/EigenHeaders.h"

// Internal module headers
#include "Connection.h"
#include "SectorCheck.h"
#include "ComputeParams.h"

#include "SatelliteUtils/catalog_tle.h"

#include "Shared/Errors.h"
#include "Shared/Messages.h"
#include "Shared/SearchArea.h"

#include "CoordinateSystems/CSUtils.h"

#include "Utils/FormatString.h"

namespace TargetEnvironment
{
  class Server
  {
    boost::asio::io_service _service;
    boost::asio::ip::tcp::endpoint _ep;
    boost::asio::ip::tcp::acceptor _acc;

    // Start/Stop flag
    bool _stopped;

    // Load config flag
    bool _config_loaded;

    // Thread pool for request proccesing and I/O
    boost::thread_group _work_threads;

    // List of clients
    std::list<Connection> _clients;

    // Clients list modification lock
    std::mutex _clients_mutex;

    // Clients counter
    std::atomic<uint32_t> _clients_counter;

    // Satellites
    CatalogTLE _satellite_catalog;

    // Start reading for specific connection.
    void start_read(std::list<Connection>::iterator);

    // Reading callback
    void on_read(std::list<Connection>::iterator, const boost::system::error_code&, size_t);

    // Start accepting new connections
    void start_accept();

    // Accept callback
    void on_accept(std::shared_ptr<boost::asio::ip::tcp::socket>, const boost::system::error_code&);

    // Process request from client
    ErrorCode process_request(const TargetEnvironment::SearchArea&, const CSUtils::GCSPoint&, const TimeType&, TargetsListMsg&);

    // Maximum number of connections.
    std::uint32_t _max_connections;

    // TLE Catalog path
    std::string _tle_path;

  public:

    Server();

    // Start server. Return OK if start was succefull, SystemError otherwise.
    ErrorCode start();

    // Stop server. Return OK if start was succefull, SystemError otherwise.
    ErrorCode stop();

    // Read and parse configuration file. Return OK if read and parsing was success. 
    // Return SystemError if file was not found and ConfigError due to parse error.
    ErrorCode load_config(std::string path_to_file);

    ~Server();
  };
}

