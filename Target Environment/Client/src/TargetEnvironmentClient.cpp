#include "../include/TargetEnvironmentClient.h"
#include "../../../Utils/include/GetValueWrapper.hpp"
#include <boost/algorithm/string.hpp>

using namespace boost::asio;
using namespace boost::system;

namespace TargetEnvironment
{
  bool TargetEnvironmentClient::parse_targets(const std::vector<TargetEnvironment::TargetDesc>& targets)
  {
    _tbl.clear();
    for (auto& item : targets)
    {
      _tbl.add_target(item);
    }
    return true;
  }

  ErrorCode TargetEnvironmentClient::close_client()
  {
    error_code ec;
    _connected = false;

    _socket.shutdown(ip::tcp::socket::shutdown_both, ec);

    if (ec)
    {
      DFLOG("Target Environment client: " + ec.message());
      return ErrorCode::Unknown_EC;
    }

    _socket.close(ec);

    if (ec)
    {
      DFLOG("Target Environment client: " + ec.message());
      return ErrorCode::Unknown_EC;
    }

    return ErrorCode::OK_EC;
  }

  TargetEnvironmentClient::TargetEnvironmentClient(const ReferencePointDesc& rf_point) :
    _service(),
    _socket(_service),
    _reference_point(rf_point),
    _connected(false)
  {
  }

  TargetEnvironmentClient::~TargetEnvironmentClient()
  {
    DFLOG("Target Environment client: Client delete.");
    if (_socket.is_open())
    {
      DFLOG("Target Environment client: Close client socket.");
      error_code ec;
      _socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);

      if (ec)
      {
        DFLOG("Target Environment client: " + ec.message());
      }
      _socket.close(ec);
      if (ec)
      {
        DFLOG("Target Environment client: " + ec.message());
      }
    }
  }

  ErrorCode TargetEnvironmentClient::connect(const ip::tcp::endpoint& server_address)
  {
    if (is_connected())
    {
      return ErrorCode::OK_EC;
    }
    
    DFLOG("Target Environment client: Connect to server.");

    error_code ec;
    _socket.connect(server_address, ec);

    if (ec)
    {
      DFLOG(ec.message());
      return ErrorCode::Unknown_EC;
    }

    DFLOG("Target Environment client: Connection success.");

    DFLOG("Target Environment client: Send reference point descriptor message.");
    RefPoint msg(_reference_point);

    write(_socket, buffer(msg.msg), ec);
    if (ec)
    {
      DFLOG("Target Environment client: " + ec.message());
      return ErrorCode::RegisterFail_EC;
    }
    DFLOG("Target Environment client: Reference point descriptor message was send successfully.");
    
    streambuf buff;
    read_until(_socket, buff, "\n", ec);
    if (ec)
    {
      DFLOG("Target Environment client: " + ec.message());
      return ErrorCode::RegisterFail_EC;
    }

    std::string res(buffer_cast<const char*>(buff.data()));
    if (res == RegisterSuccess)
    {
      DFLOG("Target Environment client: Register success.");
      _connected = true;
      return ErrorCode::OK_EC;
    }
    else if (res == RefPointFail)
    {
      DFLOG("Target Environment client: Register failed.");
      return ErrorCode::RegisterFail_EC;
    }
    else
    {
      DFLOG("Target Environment client: Unknow type of message.");
      return ErrorCode::Unknown_EC;
    }
  }

  bool TargetEnvironmentClient::is_connected()
  {
    return _connected;
  }

  ErrorCode TargetEnvironmentClient::disconnect()
  {
    DFLOG("Target Environment client: Disconnect from server.");
    return close_client();
  }

  ErrorCode TargetEnvironmentClient::get_targets(TargetTable::TableType& tbl, const TimeType& t)
  {
    DFLOG("Target Environment client: Sending request.");
    
    if (is_connected() == false)
      return ErrorCode::RequestFail_EC;

    auto dur = std::chrono::time_point_cast<std::chrono::duration<uint64_t, std::ratio_multiply<std::ratio<100, 1>, std::nano>>>(t).time_since_epoch().count();

    error_code ec;
    Request req_msg(_reference_point.client_id, dur);

    write(_socket, buffer(req_msg.msg), ec);

    if (ec)
    {
      DFLOG("Target Environment client: " + ec.message());
      close_client();
      return ErrorCode::RequestFail_EC;
    }
    DFLOG("Target Environment client: Request send successfully. Read server response.");

    streambuf buff;
    auto len = read_until(_socket, buff, "\n", ec);

    if (ec)
    {
      DFLOG("Target Environment client: " + ec.message());
      close_client();
      return ErrorCode::RequestFail_EC;
    }

    std::string res(buffer_cast<const char*>(buff.data()), len);
    if (res == RequestFail)
      return ErrorCode::RequestFail_EC;

    DFLOG("Target Environment client: Targets list read succefully.");

    std::size_t ntargets;
    
    rapidjson::Document root;
    root.Parse(res.c_str());

    if (root.HasParseError())
    {
      DFLOG("Target Environment client: Error parsing Target list message. Wrong data format.");
      return ErrorCode::RequestFail_EC;
    }

    try
    {
      ntargets = get_value<std::uint32_t>(root, "size");
    }
    catch (...)
    {
      DFLOG("Target Environment client: Error parsing Target list message. Wrong data format.");
      return ErrorCode::RequestFail_EC;
    }
    DFLOG(Utils::string_format("Target Environment client: Number of targets %u.", ntargets));

    if (ntargets > sizeof(TargetDesc) * MAX_TARGETS)
    {
      DFLOG("Target Environment client: Too many targets.");
      return ErrorCode::RequestFail_EC;
    }

    if (ntargets != 0)
    {
      DFLOG("Target Environment client: Loading targets from server.");
      streambuf buffer;
      len = read_until(_socket, buffer, '\n', ec);
      if (ec)
      {
        DFLOG("Target Environment client: " + ec.message());
        close_client();
        return ErrorCode::RequestFail_EC;
      }
      DFLOG("Target Environment client: Reading targets success.");
      DFLOG("Target Environment client: Parse targets list.");

      ;
      if (parse_targets(deserialize(std::string(buffer_cast<const char*>(buffer.data()), len))) == false)
      {
        DFLOG("Target Environment client: Parse targets message error.");
        return ErrorCode::RequestFail_EC;
      }
    }

    tbl = _tbl.get_table();
    
    return ErrorCode::OK_EC;
  }

  std::vector<TargetEnvironment::TargetDesc> TargetEnvironmentClient::deserialize(const std::string& msg)
  {
    std::vector<TargetEnvironment::TargetDesc> ret;

    if (msg.empty())
      return ret;

    std::size_t item_end = 0;

    while (true)
    {
      auto item_start = item_end;
      item_end = msg.find(";", item_start + 1);

      if (item_end == std::string::npos)
        break;

      std::string item_str = msg.substr(item_start + 1, item_end - item_start - 1);

      TargetDesc item;

      std::vector<std::string> tokens;
      boost::split(tokens, item_str, boost::is_any_of(" ;:"));

      if (tokens.size() != 23)
      {
        DFLOG("Target Environment client: deserialize error. Wrong message format.");
        return ret;
      }

      try
      {
        item.target_id = std::stoi(tokens[1]);

        item.x[0] = std::stod(tokens[3]);
        item.x[1] = std::stod(tokens[4]);
        item.x[2] = std::stod(tokens[5]);
        item.x[3] = std::stod(tokens[6]);
        item.x[4] = std::stod(tokens[7]);
        item.x[5] = std::stod(tokens[8]);

        item.y[0] = std::stod(tokens[10]);
        item.y[1] = std::stod(tokens[11]);
        item.y[2] = std::stod(tokens[12]);
        item.y[3] = std::stod(tokens[13]);
        item.y[4] = std::stod(tokens[14]);
        item.y[5] = std::stod(tokens[15]);

        item.z[0] = std::stod(tokens[17]);
        item.z[1] = std::stod(tokens[18]);
        item.z[2] = std::stod(tokens[19]);
        item.z[3] = std::stod(tokens[20]);
        item.z[4] = std::stod(tokens[21]);
        item.z[5] = std::stod(tokens[22]);
      }
      catch (...)
      {
        DFLOG("Target Environment client: deserialize error. Parsing message error.");
        return ret;
      }

      ret.emplace_back(item);
    }

    return ret;
  }
}
