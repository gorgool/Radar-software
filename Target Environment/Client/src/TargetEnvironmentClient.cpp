#include "../include/TargetEnvironmentClient.h"
#include <random>

using namespace boost::asio;
using namespace boost::system;

namespace TargetEnvironment
{
  bool TargetEnvironmentClient::parse_targets(const char * msg, const std::size_t ntargets)
  {
    _tbl.clear();
    for (size_t idx = 0; idx < ntargets; ++idx)
    {
      const TargetDesc* target_ptr = reinterpret_cast<const TargetDesc*>(msg);

      TargetDesc buff;
      memcpy(reinterpret_cast<char*>(&buff), target_ptr, sizeof(TargetDesc));

      _tbl.add_target(buff);
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
      DLOG("Target Environment client: " + ec.message());
      return ErrorCode::Unknown_EC;
    }

    _socket.close(ec);

    if (ec)
    {
      DLOG("Target Environment client: " + ec.message());
      return ErrorCode::Unknown_EC;
    }

    return ErrorCode::OK_EC;
  }

  TargetEnvironmentClient::TargetEnvironmentClient() :
    _service(),
    _socket(_service),
    _connected(false)
  {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<std::size_t> id(0, std::numeric_limits<std::size_t>::max());
    auto client_id = id(gen);

    _reference_point = ReferencePointDesc(client_id, 44.392087, -68.204052, 100, 2000000);
  }

  TargetEnvironmentClient::~TargetEnvironmentClient()
  {
    DLOG("Target Environment client: Client delete.");
    if (_socket.is_open())
    {
      DLOG("Target Environment client: Close client socket.");
      error_code ec;
      _socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
      // Throw an error when attempting to shutdown connection on 
      // server side with failed server connection
      if (ec)
      {
        DLOG("Target Environment client: " + ec.message());
      }
      _socket.close(ec);
      if (ec)
      {
        DLOG("Target Environment client: " + ec.message());
      }
    }
  }

  ErrorCode TargetEnvironmentClient::connect(const ip::tcp::endpoint& server_address)
  {
    if (is_connected())
    {
      return ErrorCode::OK_EC;
    }
    
    DLOG("Target Environment client: Connect to server.");

    error_code ec;
    _socket.connect(server_address, ec);
    // TODO: Connection time out!!!

    if (ec)
    {
      DLOG(ec.message());
      return ErrorCode::Unknown_EC;
    }

    DLOG("Target Environment client: Connection success.");

    DLOG("Target Environment client: Send reference point descriptor message.");
    RefPoint msg(
      _reference_point.client_id, 
      _reference_point.latitude, 
      _reference_point.longitude, 
      _reference_point.height,
      _reference_point.range);

    write(_socket, buffer(msg.msg), ec);
    if (ec)
    {
      DLOG("Target Environment client: " + ec.message());
      return ErrorCode::RegisterFail_EC;
    }
    DLOG("Target Environment client: Reference point descriptor message was send successfully.");
    
    streambuf buff;
    read_until(_socket, buff, "\n", ec);
    if (ec)
    {
      DLOG("Target Environment client: " + ec.message());
      return ErrorCode::RegisterFail_EC;
    }

    std::string res(buffer_cast<const char*>(buff.data()));
    if (res == RegisterSuccess)
    {
      DLOG("Target Environment client: Register success.");
      _connected = true;
      return ErrorCode::OK_EC;
    }
    else if (res == RefPointFail)
    {
      DLOG("Target Environment client: Register failed.");
      return ErrorCode::RegisterFail_EC;
    }
    else
    {
      DLOG("Target Environment client: Unknow type of message.");
      return ErrorCode::Unknown_EC;
    }
  }

  bool TargetEnvironmentClient::is_connected()
  {
    return _connected;
  }

  ErrorCode TargetEnvironmentClient::disconnect()
  {
    DLOG("Target Environment client: Disconnect from server.");
    return close_client();
  }

  ErrorCode TargetEnvironmentClient::get_targets(TargetTable::TableType& tbl, const TimeType& t)
  {
    DLOG("Target Environment client: Sending request.");
    
    if (is_connected() == false)
      return ErrorCode::RequestFail_EC;

    ClockType::duration::rep dur = t.time_since_epoch().count();

    error_code ec;
    Request req_msg(_reference_point.client_id, dur);

    write(_socket, buffer(req_msg.msg), ec);

    if (ec)
    {
      DLOG("Target Environment client: " + ec.message());
      close_client();
      return ErrorCode::RequestFail_EC;
    }
    DLOG("Target Environment client: Request send successfully. Read server response.");

    streambuf buff;
    read_until(_socket, buff, "\n", ec);

    if (ec)
    {
      DLOG("Target Environment client: " + ec.message());
      close_client();
      return ErrorCode::RequestFail_EC;
    }

    std::string res(buffer_cast<const char*>(buff.data()));
    if (res == RequestFail)
      return ErrorCode::RequestFail_EC;

    DLOG("Target Environment client: Targets list read succefully.");

    const std::size_t ntargets = std::stoi(res.substr(res.find(":") + 1, res.find(";")));
    const std::size_t nbytes = ntargets * sizeof(TargetDesc);

    DLOG(Utils::string_format("Target Environment client: Number of targets %u.", ntargets));

    if (ntargets > sizeof(TargetDesc) * MAX_TARGETS)
    {
      DLOG("Target Environment client: Too many targets.");
      return ErrorCode::RequestFail_EC;
    }

    if (ntargets != 0)
    {
      DLOG("Target Environment client: Loading targets from server.");
      streambuf raw_buffer;
      boost::asio::read(_socket, raw_buffer, boost::asio::transfer_exactly(nbytes), ec);
      if (ec)
      {
        DLOG("Target Environment client: " + ec.message());
        close_client();
        return ErrorCode::RequestFail_EC;
      }
      DLOG("Target Environment client: Reading targets success.");
      DLOG("Target Environment client: Parse targets list.");
      if (parse_targets(boost::asio::buffer_cast<const char*>(raw_buffer.data()), ntargets) == false)
      {
        DLOG("Target Environment client: Parse targets message error.");
        return ErrorCode::RequestFail_EC;
      }
    }

    tbl = _tbl.get_table();
    
    return ErrorCode::OK_EC;
  }
}
