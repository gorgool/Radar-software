#pragma once

#include <ctime>
#include <inttypes.h>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "../../../Utils/include/EigenHeaders.h"
#include <list>
#include <cstddef>
#include "Connection.h"
#include "../../Satellite_Utils/include/catalog_tle.h"
#include "../../Shared/include/TargetEnvironmentErrors.h"
#include "../../Shared/include/TargetEnvironmentMessages.h"
#include "../../Shared/include/ReferencePointDesc.h"
#include "../../../Utils/include/FormatString.h"
#include "../../../Utils/include/SplitString.hpp"
#include "../../../CoordinateSystems/include/CSUtils.h"
#include "ComputeParams.h"

#include "../../../Utils/include/Logger.h"

namespace TargetEnvironment
{
  /*
      Check if target (point) enter in sector (params). Return true if hit, false otherwise
  */
  bool sector_check(const Eigen::Vector3d& point, const ReferencePointDesc& params);

  class TargetEnvironmentServer
  {
    boost::asio::io_service& _service;
    boost::asio::ip::tcp::endpoint _ep;
    boost::asio::ip::tcp::acceptor _acc;

    boost::thread_group _work_threads;

    std::list<Connection> _clients;

    CatalogTLE _satellite_catalog;

    void start_read(std::list<Connection>::iterator conn);
    void on_read(std::list<Connection>::iterator, const boost::system::error_code&, size_t);
    void accept_connection();
    void on_accept(std::shared_ptr<boost::asio::ip::tcp::socket>, const boost::system::error_code&);
    ErrorCode process_request(std::list<Connection>::iterator, const TimeType& time);
    ErrorCode load_config();
    bool _stop_flag;

  public:

    TargetEnvironmentServer(boost::asio::io_service&);

    // Return true if server is running.
    bool is_stoped();

    // Start server. Return OK_EC if start was succefull, Unknown_EC otherwise.
    ErrorCode start();

    // Stop server. Return OK_EC if start was succefull, Unknown_EC otherwise.
    ErrorCode stop();

    ~TargetEnvironmentServer();
  };
}

