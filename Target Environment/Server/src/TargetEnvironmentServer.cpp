#include "../include/TargetEnvironmentServer.h"
#include "../../../Utils/include/GetValueWrapper.hpp"
#include <omp.h>

namespace TargetEnvironment
{
  using namespace boost::asio;
  using namespace boost::system;

  void TargetEnvironmentServer::on_read(std::list<Connection>::iterator conn, const boost::system::error_code & ec, size_t size)
  {
    if (ec)
    {
      // Check shutdown of connection
      if (error::eof == ec || error::connection_reset == ec)
      {
        DFLOG("Target Environment Server: Deleting client.");
        conn->shutdown();
        _clients.erase(conn);
        return;
      }
      DFLOG("Target Environment Server: " + ec.message());
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
          DFLOG("Target Environment Server: " + ec.message());
        }
      });
    };

    // Parse message
    std::string res(buffer_cast<const char*>(conn->read_buffer.get()->data()), size);

    // Clear read buffer
    conn->read_buffer.get()->consume(size);

    std::string msg_header;
    rapidjson::Document root;
    root.Parse(res.c_str());

    if (root.HasParseError())
    {
      DFLOG("Target Environment Server: Message parse error. Empty message.");
      start_read(conn);
      return;
    }

    try
    {
      msg_header = get_value<std::string>(root, "message");
    }
    catch (...)
    {
      DFLOG("Target Environment Server: Request message error. Wrong data format.");
      send_msg(RequestFail);
      start_read(conn);
      return;
    }

    if (msg_header == "RefPoint")
    {
      try
      {
        // Parse Reference point message
        auto& ref_point = conn->ref_point;
        ref_point.client_id = get_value<std::uint32_t>(root, "id");
        ref_point.latitude = get_value<double>(root, "lat");
        ref_point.longitude = get_value<double>(root, "lon");
        ref_point.height = get_value<double>(root, "alt");
        ref_point.range = get_value<double>(root, "range");
        ref_point.azimuth_ang = get_value<double>(root, "azimuth_angle");
        ref_point.azimuth_width = get_value<double>(root, "azimuth_width");
        ref_point.elevation_ang = get_value<double>(root, "elevation_angle");
        ref_point.elevation_width = get_value<double>(root, "elevation_width");
      }

      catch (...)
      {
        DFLOG("Target Environment Server: Request message error. Wrong data format.");
        send_msg(RequestFail);
        start_read(conn);
        return;
      }

      DFLOG("Target Environment Server: Register success. Send confirmation to client.");
      send_msg(RegisterSuccess);
      start_read(conn);
      return;
    }
    else if (msg_header == "Request")
    {
      TimeType time;

      try
      {
        std::chrono::duration<uint64_t, std::ratio_multiply<std::ratio<100, 1>, std::nano>> dur(get_value<std::uint64_t>(root, "time"));
        time = std::chrono::time_point<ClockType, std::chrono::duration<uint64_t, std::ratio_multiply<std::ratio<100, 1>, std::nano>>>(dur);
      }

      catch (...)
      {
        DFLOG("Target Environment Server: Request message error. Wrong data format.");
        send_msg(RequestFail);
        start_read(conn);
        return;
      }

      auto err = process_request(conn, time);
      if (err != OK_EC)
      {
        DFLOG("Target Environment Server: Error processing request.");
        send_msg(RequestFail);
      }
      start_read(conn);
      return;
    }
    else
    {
      DFLOG("Target Environment Server: Unknown type of message.");
      start_read(conn);
      return;
    }
  }

  void TargetEnvironmentServer::start_read(std::list<Connection>::iterator conn)
  {
    async_read_until(*conn->socket, *conn->read_buffer, "\n",
      std::bind(&TargetEnvironmentServer::on_read, this, conn, std::placeholders::_1, std::placeholders::_2));
  }

  void TargetEnvironmentServer::accept_connection()
  {
    DFLOG("Target Environment Server: Accepting connections...");
    std::shared_ptr<ip::tcp::socket> accept_socket(new ip::tcp::socket(_service));

    _acc.async_accept(
      *accept_socket,
      std::bind(&TargetEnvironmentServer::on_accept, this, accept_socket, std::placeholders::_1));
  }

  void TargetEnvironmentServer::on_accept(std::shared_ptr<ip::tcp::socket> socket, const error_code& ec)
  {
    if (ec)
    {
      // If accept operation was canceled. Ignore error
      if (ec == error::operation_aborted)
      {
        return;
      }
      DFLOG("Target Environment Server: " + ec.message());
      return;
    }
    DFLOG("Target Environment Server: Connection accepted.");

    std::list<Connection>::iterator conn = _clients.insert(_clients.end(), Connection(socket));

    async_read_until(*conn->socket, *conn->read_buffer, "\n",
      std::bind(&TargetEnvironmentServer::on_read, this, conn, std::placeholders::_1, std::placeholders::_2));

    accept_connection();
  }

  ErrorCode TargetEnvironmentServer::process_request(std::list<Connection>::iterator conn, const TimeType& time)
  {
    DFLOG(Utils::string_format("Target Environment Server: Processing request from client. Id : %u", conn->ref_point.client_id));

    std::vector<TargetDesc> target_buffer;

    DFLOG("Target Environment Server: Read and filter satellites.");
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
      Eigen::Vector3d point(v0.x, v0.y, v0.z);

      if (sector_check(point, conn->ref_point))
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

    DFLOG(Utils::string_format("Target Environment Server: Found %u satellites for client %u.", target_buffer.size(), conn->ref_point.client_id));

    // Sending result back to client
    size_t count = target_buffer.size();
    TargetsList msg(count);
    error_code ec;
    write(*conn->socket, buffer(msg.msg), ec);
    if (ec)
    {
      DFLOG("Target Environment Server: " + ec.message());
      return Unknown_EC;
    }

    if (count > 0)
    {
      auto msg = serialize(target_buffer);
      write(*conn->socket, buffer(msg), ec);

      if (ec)
      {
        DFLOG("Target Environment Server: " + ec.message());
        return Unknown_EC;
      }
    }

    return OK_EC;
  }

  ErrorCode TargetEnvironmentServer::load_config()
  {
    try
    {
      boost::property_tree::ptree cfg;
      boost::property_tree::read_json("etc/settings.cfg", cfg);
      uint16_t port = cfg.get<uint16_t>("listen_port");

      _ep = ip::tcp::endpoint(ip::tcp::v4(), port);

      return OK_EC;
    }
    catch (...)
    {
      return Unknown_EC;
    }
  }

  TargetEnvironmentServer::TargetEnvironmentServer(boost::asio::io_service& s) :
    _service(s),
    _acc(s),
    _stop_flag(true)
  {}

  ErrorCode TargetEnvironmentServer::start()
  {
    DFLOG("Target Environment Server: Server start.");
    setlocale(LC_ALL, "");

    auto err = load_config();
    if (err != OK_EC)
    {
      DFLOG("Target Environment Server: Error while reading or parsing configuration file.");
      return Unknown_EC;
    }

    DFLOG("Target Environment Server: Loading satellite catalog.");
    if (_satellite_catalog.parse_keo("Resource/catalog.tle") == false)
    {
      DFLOG("Target Environment Server: Error parsing tle file.");
      return Unknown_EC;
    }
    DFLOG("Target Environment Server: Catalog load success.");

    error_code ec;

    _acc.open(_ep.protocol(), ec);
    if (ec)
    {
      DFLOG("Target Environment Server: " + ec.message());
      return Unknown_EC;
    }
    _acc.bind(_ep, ec);
    if (ec)
    {
      DFLOG("Target Environment Server: " + ec.message());
      return Unknown_EC;
    }
    _acc.listen();

    accept_connection();

    DFLOG("Target Environment Server: Starting work threads...");
    for (size_t n = 0; n < boost::thread::hardware_concurrency(); ++n)
    {
      _work_threads.create_thread([this]()
      {
        DFLOG("Target Environment Server: Work thread started.");
        setlocale(LC_ALL, "");
        _service.run();
        DFLOG("Target Environment Server: Work thread stoped.");
      });
    }

    _stop_flag = false;

    return OK_EC;
  }

  ErrorCode TargetEnvironmentServer::stop()
  {
    DFLOG("Target Environment Server: Server stop.");
    error_code ec;

    DFLOG("Target Environment Server: Clear satellite catalog.");
    _satellite_catalog.clear_satellite_list();

    DFLOG("Target Environment Server: Closing acceptor.");
    if (_acc.is_open())
    {
      _acc.cancel(ec);
      if (ec)
      {
        DFLOG("Target Environment Server: " + ec.message());
        return Unknown_EC;
      }
      _acc.close(ec);
      if (ec)
      {
        DFLOG("Target Environment Server: " + ec.message());
        return Unknown_EC;
      }
    }

    DFLOG("Target Environment Server: Close coneections for clients.");
    for (auto& conn : _clients)
    {
      if (conn.socket.get()->is_open())
      {
        conn.socket.get()->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
        conn.socket.get()->close(ec);
        if (ec)
        {
          DFLOG("Target Environment Server: " + ec.message());
        }
      }
    }
    _clients.clear();

    _service.stop();

    _service.reset();

    DFLOG("Target Environment Server: Waiting for work threads.");
    _work_threads.join_all();
    DFLOG("Target Environment Server: Work threads closed.");
    _stop_flag = true;
    return OK_EC;
  }

  TargetEnvironmentServer::~TargetEnvironmentServer()
  {
    stop();
  }

  bool TargetEnvironmentServer::is_stoped()
  {
    return _stop_flag;
  }

  std::string TargetEnvironmentServer::serialize(std::vector<TargetEnvironment::TargetDesc>& targets)
  {
    std::string ret;

    for (auto& item : targets)
    {
      std::string item_buff = "id:" + std::to_string(item.target_id);

      item_buff += " x:" + std::to_string(item.x[0]) + " "
        + std::to_string(item.x[1]) + " "
        + std::to_string(item.x[2]) + " "
        + std::to_string(item.x[3]) + " "
        + std::to_string(item.x[4]) + " "
        + std::to_string(item.x[5]);

      item_buff += " y:" + std::to_string(item.y[0]) + " "
        + std::to_string(item.y[1]) + " "
        + std::to_string(item.y[2]) + " "
        + std::to_string(item.y[3]) + " "
        + std::to_string(item.y[4]) + " "
        + std::to_string(item.y[5]);

      item_buff += " z:" + std::to_string(item.z[0]) + " "
        + std::to_string(item.z[1]) + " "
        + std::to_string(item.z[2]) + " "
        + std::to_string(item.z[3]) + " "
        + std::to_string(item.z[4]) + " "
        + std::to_string(item.z[5]) + ";";

      ret += item_buff;
    }

    return ret + '\n';
  }
}
