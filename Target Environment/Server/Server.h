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
#include "ConfigManager/ConfigManager.hpp"

#include "Logger/Logger.h"

namespace TargetEnvironment
{
  class Server
  {
    /** @brief The boost asio service. */
    boost::asio::io_service _service;
    /** @brief The endpoint - ip address and port pair. */
    boost::asio::ip::tcp::endpoint _ep;
    /** @brief The acceptor socket. */
    boost::asio::ip::tcp::acceptor _acc;

    /** @brief true if stopped. */
    bool _stopped;

    /** @brief true if configuration loaded. */
    bool _config_loaded;

    /** @brief The work threads for request proccesing and I/O. */
    boost::thread_group _work_threads;

    /** @brief The list of clients. */
    std::list<Connection> _clients;

    /** @brief The clients list modification mutex. */
    std::mutex _clients_mutex;

    /** @brief The clients counter. */
    std::atomic<uint32_t> _clients_counter;

    /** @brief The satellite catalog. */
    CatalogTLE _satellite_catalog; 

    /**
     * @fn  void Server::start_read(std::list<Connection>::iterator conn);
     *
     * @brief Starts reading from socket for specific connection.
     *
     * @author  Gorgool
     * @date  20.04.2016
     *
     * @param conn  The connection iterator.
     */

    void start_read(std::list<Connection>::iterator conn);

    /**
     * @fn  void Server::on_read(std::list<Connection>::iterator conn, const boost::system::error_code& err, size_t s);
     *
     * @brief Executes the read action.
     *
     * @author  Gorgool
     * @date  20.04.2016
     *
     * @param conn  The connection iterator.
     * @param err   The error code.
     * @param s     The size of received message.
     */

    void on_read(std::list<Connection>::iterator conn, const boost::system::error_code& err, size_t s);

    /**
     * @fn  void Server::start_accept();
     *
     * @brief Starts an accepting of new connections.
     *
     * @author  Gorgool
     * @date  20.04.2016
     */

    void start_accept();

    /**
     * @fn  void Server::on_accept(std::shared_ptr<boost::asio::ip::tcp::socket> socket, const boost::system::error_code& err);
     *
     * @brief Executes the accept action.
     *
     * @author  Gorgool
     * @date  20.04.2016
     *
     * @param socket  The socket.             
     * @param err     The error code.
     */

    void on_accept(std::shared_ptr<boost::asio::ip::tcp::socket> socket, const boost::system::error_code& err);

    /**
     * @fn  ErrorCode Server::process_request(const TargetEnvironment::SearchArea& area, const CSUtils::GCSPoint& radar_loc, const TimeType& time, TargetsListMsg& ret_targets);
     *
     * @brief Process the request.
     *        Return list of targets thats are found in given search area.
     *
     * @author  Gorgool
     * @date  20.04.2016
     *
     * @param area                  The search area descriptor.
     * @param radar_loc             The radar location.
     * @param time                  Time.
     * @param [out]  ret_targets    List of targets in given area.
     *
     * @return  An ErrorCode.
     */

    ErrorCode process_request(const TargetEnvironment::SearchArea& area, const CSUtils::GCSPoint& radar_loc, const TimeType& time, TargetsListMsg& ret_targets);

    /** @brief The maximum connections. */
    std::uint32_t _max_connections;

    /** @brief Full pathname of the tle catalog file. */
    std::string _tle_path;

  public:

    Server();

    /**
     * @fn  ErrorCode Server::start();
     *
     * @brief Start server.
     *
     * @author  Gorgool
     * @date  20.04.2016
     *
     * @return  OK if start was succefull, SystemError otherwise.
     */

    ErrorCode start();

    /**
     * @fn  ErrorCode Server::stop();
     *
     * @brief Stop server.
     *
     * @author  Gorgool
     * @date  20.04.2016
     *
     * @return  OK if start was succefull, SystemError otherwise.
     */

    ErrorCode stop();

    /**
     * @fn  ErrorCode Server::load_config(const ConfigManager& conf);
     *
     * @brief Loads a configuration.
     *
     * @author  Gorgool
     * @date  20.04.2016
     *
     * @param conf  The conf.
     *
     * @return  OK if read and parsing was success. Return SystemError if file was not found and ConfigError due to parse error.
     */

    ErrorCode load_config(const ConfigManager& conf);

    ~Server();
  };
}

