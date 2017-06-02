#include "Server.h"
#include "Utils/GetValueWrapper.hpp"
#include <omp.h>
#include <sstream>

namespace TargetEnvironment
{
  using namespace boost::asio;
  using namespace boost::system;

  Server::Server() :
    _service(),
    _acc(_service),
    _stopped(true),
    _config_loaded(false),
    _clients_counter(0)
  {
    setlocale(LC_ALL, "");
  }

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

  ErrorCode Server::load_config(const ConfigManager& conf)
  {
    if (_stopped == false)
    {
      Logger::log_dispfile(SeverityLevel::Error, "Target Environment Server", "Can't load config, server is running now.");
      return ErrorCode::SystemError;
    }

    try
    {
      auto& section = conf.get_section("te_settings");
      auto port = conf.get_value<std::uint32_t>(section, "port");
      _ep = ip::tcp::endpoint(ip::tcp::v4(), port);

      _max_connections = conf.get_value<std::uint32_t>(section, "max_connections");

      _tle_path = conf.get_value<std::string>(section, "tle_catalog");

      _config_loaded = true;

      return ErrorCode::OK;
    }
    catch (const SystemException& ex)
    {
      Logger::log_dispfile(SeverityLevel::Error, "Target Environment Server", ex.what());
      return ErrorCode::SystemError;
    }
    catch (const ConfigException& ex)
    {
      Logger::log_dispfile(SeverityLevel::Error, "Target Environment Server", ex.what());
      return ErrorCode::ConfigError;
    }
  }

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

  void Server::on_read(std::list<Connection>::iterator conn, const boost::system::error_code & ec, size_t size)
  {
    if (ec)
    {
      // Check shutdown of connection
      if (error::eof == ec || error::connection_reset == ec)
      {
        Logger::log_dispfile(SeverityLevel::Notice, "Target Environment Server", "Deleting client.");
        conn->shutdown();

        _clients_mutex.lock();
        _clients.erase(conn);
        _clients_mutex.unlock();

        _clients_counter--;

        return;
      }
      Logger::log_dispfile(SeverityLevel::Error, "Target Environment Server", ec.message());
      return;
    }

    // Message sender without call back
    auto send_msg = [&](const std::string& text)
    {
      async_write(*conn->socket, buffer(text),
        [](const error_code& ec, std::size_t)
      {
        if (ec)
        {
          Logger::log_dispfile(SeverityLevel::Error, "Target Environment Server", ec.message());
        }
      });
    };

    // Parse message
    std::string res(buffer_cast<const char*>(conn->read_buffer.get()->data()), size);

    // Clear read buffer
    conn->read_buffer.get()->consume(size);

    rapidjson::Document root;
    root.Parse(res.c_str());

    if (root.HasParseError() || !root.IsObject())
    {
      Logger::log_dispfile(SeverityLevel::Error, "Target Environment Server", "Message parse error. Empty message.");
      send_msg(RequestFailMsg);
      start_read(conn);
      return;
    }

    SearchArea sa;
    CSUtils::GCSPoint radar_loc;
    TimeType time;
    std::string msg_header;

    try
    {
      msg_header = get_value<std::string>(root, "message");
      if (msg_header != "TE_Request")
      {
        Logger::log_dispfile(SeverityLevel::Error, "Target Environment Server", "Request message error. Wrong data format.");
        send_msg(RequestFailMsg);
        start_read(conn);
        return;
      }

      // Parse request message
      radar_loc.latitude = get_value<double>(root, "latitude");
      radar_loc.longitude = get_value<double>(root, "longitude");
      radar_loc.altitude = get_value<double>(root, "altitude");
      sa.x = get_value<double>(root, "X");
      sa.y = get_value<double>(root, "Y");
      sa.z = get_value<double>(root, "Z");
      sa.range = get_value<double>(root, "range");
      sa.azimuth_angle = get_value<double>(root, "azimuth_angle");
      sa.azimuth_width = get_value<double>(root, "azimuth_width");
      sa.elevation_angle = get_value<double>(root, "elevation_angle");
      sa.elevation_width = get_value<double>(root, "elevation_width");

      DurationType dur(get_value<std::uint64_t>(root, "time"));
      time = TimeType(dur);
    }
    catch (...)
    {
      Logger::log_dispfile(SeverityLevel::Error, "Target Environment Server", "Request message error. Wrong data format.");
      send_msg(RequestFailMsg);
      start_read(conn);
      return;
    }

    // Attention : request processing running in this thread.
    TargetsListMsg response;
    auto err = process_request(sa, radar_loc, time, response);
    if (err != OK)
    {
      Logger::log_dispfile(SeverityLevel::Error, "Target Environment Server", "Error processing request.");
      send_msg(RequestFailMsg);
    }

    error_code ec_temp;
    write(*conn->socket, buffer(response.msg), ec_temp);
    if (ec)
    {
      Logger::log_dispfile(SeverityLevel::Error, "Target Environment Server", ec.message());
    }

    start_read(conn);
    return;
  }

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

  void Server::start_read(std::list<Connection>::iterator conn)
  {
    async_read_until(*conn->socket, *conn->read_buffer, "\n",
      std::bind(&Server::on_read, this, conn, std::placeholders::_1, std::placeholders::_2));
  }

  /**
  * @fn  void Server::start_accept();
  *
  * @brief Starts an accepting of new connections.
  *
  * @author  Gorgool
  * @date  20.04.2016
  */

  void Server::start_accept()
  {
    std::shared_ptr<ip::tcp::socket> accept_socket(new ip::tcp::socket(_service));

    _acc.async_accept(
      *accept_socket,
      std::bind(&Server::on_accept, this, accept_socket, std::placeholders::_1));
  }

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

  void Server::on_accept(std::shared_ptr<ip::tcp::socket> socket, const error_code& ec)
  {
    if (ec)
    {
      // If accept operation was canceled. Ignore error
      if (ec == error::operation_aborted)
      {
        return;
      }
      Logger::log_dispfile(SeverityLevel::Error, "Target Environment Server", ec.message());
      return;
    }

    
    if (_clients_counter == _max_connections)
    {
      Logger::log_dispfile(SeverityLevel::Notice, "Target Environment Server", "Connection refused. Maximum number of connections.");
      
      error_code ec;
      write(*socket, buffer(RegisterFailMsg), ec);
      if (ec)
      {
        Logger::log_dispfile(SeverityLevel::Error, "Target Environment Server", "Target Environment Server: " + ec.message());
      }
    }
    else
    {
      Logger::log_dispfile(SeverityLevel::Notice, "Target Environment Server", "Connection accepted.");

      std::list<Connection>::iterator conn;
      try
      {
        std::lock_guard<std::mutex> lg(_clients_mutex);
        conn = _clients.insert(_clients.end(), Connection(socket));
        _clients_counter++;
      }
      catch (...)
      {
        Logger::log_dispfile(SeverityLevel::Error, "Target Environment Server", "Connection insert failed.");
        start_accept();
        return;
      }

      start_read(conn);

      error_code ec;
      write(*socket, buffer(RegisterSuccessMsg), ec);
      if (ec)
      {
        Logger::log_dispfile(SeverityLevel::Error, "Target Environment Server", ec.message());
      }
    }
    
    start_accept();
  }

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

  ErrorCode Server::process_request(const SearchArea& area, const CSUtils::GCSPoint& radar_location,const TimeType& time, TargetsListMsg& result)
  {
    std::vector<Target> target_buffer;
    auto satellite_list = _satellite_catalog.get_satellite_list();

    // Time and date in Julian time
    auto temp = ClockType::to_time_t(time);
    std::tm sat_date;
#ifdef __linux__
    gmtime_r(&temp, &sat_date);
#endif
#ifdef _WIN32
    gmtime_s(&sat_date, &temp);
#endif
    // In days
    double ref_time = Utils::Julian_date(
      sat_date.tm_year + 1900,
      sat_date.tm_mon + 1,
      sat_date.tm_mday,
      sat_date.tm_hour + sat_date.tm_min / 60.0 + sat_date.tm_sec / 60.0);

    #pragma omp parallel for
    for (int idx = 0; idx < satellite_list.size(); ++idx)
    {
      auto item = satellite_list[idx];
      // Elapsed time in minutes
      double elapced_time = (ref_time - item.get()->jd) * 24.0 * 60.0;
      auto compute_position = item.get()->norad;
      tVECT3 v0, v1; // Target coordinates and velocity in Geodetic CS
      compute_position->getPosition(elapced_time, v0, v1, NoradBase::CSType::cs_GSC);
      auto point = CSUtils::ECEFPoint(v0.x, v0.y, v0.z);

      if (sector_check(point, area, radar_location))
      {
        // If target in sector

        // Compute 5 more points with offsets +10 +20 +30 +40 +50 seconds
        std::array<double, 6> x_points;
        x_points[0] = v0.x;
        std::array<double, 6> y_points;
        y_points[0] = v0.y;
        std::array<double, 6> z_points;
        z_points[0] = v0.z;

        const double time_offset = 10.0;

        for (std::size_t n_point = 1; n_point < 6; ++n_point)
        {
          compute_position->getPosition(elapced_time + (time_offset * n_point) / 60.0, v0, v1, NoradBase::CSType::cs_GSC);
          x_points[n_point] = v0.x;
          y_points[n_point] = v0.y;
          z_points[n_point] = v0.z;
        }

        auto desc = compute_params(item.get()->number, x_points, y_points, z_points);

        #pragma omp critical
        {
          target_buffer.push_back(desc);
        }
      }
    }

    result = TargetsListMsg(target_buffer);

    return OK;
  }

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

  ErrorCode Server::start()
  {
    if (_stopped == false)
    {
      Logger::log_dispfile(SeverityLevel::Error, "Target Environment Server", "Server already running.");
      return ErrorCode::SystemError;
    }

    if (_config_loaded == false)
    {
      Logger::log_dispfile(SeverityLevel::Error, "Target Environment Server", "Config not loaded.");
      return ErrorCode::SystemError;
    }

    if (_satellite_catalog.parse_keo(_tle_path) == false)
    {
      Logger::log_dispfile(SeverityLevel::Error, "Target Environment Server", "Error parsing tle file.");
      return ErrorCode::ConfigError;
    }
    
    error_code ec;
    _acc.open(_ep.protocol(), ec);
    if (ec)
    {
      Logger::log_dispfile(SeverityLevel::Error, "Target Environment Server", ec.message());
      return ErrorCode::SystemError;
    }

    _acc.bind(_ep, ec);
    if (ec)
    {
      Logger::log_dispfile(SeverityLevel::Error, "Target Environment Server", ec.message());
      _acc.close(ec);
      if (ec)
      {
        Logger::log_dispfile(SeverityLevel::Error, "Target Environment Server", ec.message());
      }
      return ErrorCode::SystemError;
    }

    _acc.listen();
    start_accept();

    // Starting work threads
    for (size_t n = 0; n < boost::thread::hardware_concurrency(); ++n)
    {
      _work_threads.create_thread([this]()
      {
        setlocale(LC_ALL, "");
        _service.run();
      });
    }

    _stopped = false;

    Logger::log_dispfile(SeverityLevel::Notice, "Target Environment Server", "Server started.");

    return ErrorCode::OK;
  }

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

  ErrorCode Server::stop()
  {
    error_code ec;
    _satellite_catalog.clear_satellite_list();

    if (_acc.is_open())
    {
      _acc.cancel(ec);
      if (ec)
      {
        Logger::log_dispfile(SeverityLevel::Error, "Target Environment Server", ec.message());
        return ErrorCode::SystemError;
      }
      _acc.close(ec);
      if (ec)
      {
        Logger::log_dispfile(SeverityLevel::Error, "Target Environment Server", ec.message());
        return ErrorCode::SystemError;
      }
    }

    for (auto& conn : _clients)
    {
      if (conn.socket.get()->is_open())
      {
        conn.socket.get()->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
        conn.socket.get()->close(ec);
        if (ec)
        {
          Logger::log_dispfile(SeverityLevel::Error, "Target Environment Server", ec.message());
        }
      }
    }

    _clients.clear();
    _service.stop();
    _service.reset();

    _work_threads.join_all();
    _stopped = true;

    Logger::log_dispfile(SeverityLevel::Notice, "Target Environment Server", "Server stopped.");

    return ErrorCode::OK;
  }

  Server::~Server()
  {
    if (_stopped == false)
      stop();
  }
}
