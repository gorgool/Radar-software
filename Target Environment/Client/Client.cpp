#include "Client.h"
#include "Utils/GetValueWrapper.hpp"
#include <boost/algorithm/string.hpp>

#include "Utils/Logger.h"

using namespace boost::asio;
using namespace boost::system;

namespace TargetEnvironment
{
  Client::Client() :
    _service(),
    _socket(_service),
    _connected(false),
    _config_loaded(false)
  {
    setlocale(LC_ALL, "");
  }

  Client::~Client()
  {
    if (_socket.is_open())
    {
      error_code ec;
      _socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);

      if (ec)
      {
        Utils::Log.log("Target Environment client: " + ec.message());
      }
      _socket.close(ec);
      if (ec)
      {
        Utils::Log.log("Target Environment client: " + ec.message());
      }
    }
  }

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

  ErrorCode Client::close_client()
  {
    error_code ec;
    _connected = false;

    _socket.shutdown(ip::tcp::socket::shutdown_both, ec);

    if (ec)
    {
      Utils::Log.log("Target Environment client: " + ec.message());
      return ErrorCode::SystemError;
    }

    _socket.close(ec);

    if (ec)
    {
      Utils::Log.log("Target Environment client: " + ec.message());
      return ErrorCode::SystemError;
    }

    Utils::Log.log("Target Environment client: Client dissconnected.");
    return ErrorCode::OK;
  }

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

  ErrorCode Client::load_config(const ConfigManager& conf)
  {
    if (_connected)
    {
      Utils::Log.log("Target Environment Client: Can't load config, client already connected.");
      return ErrorCode::SystemError;
    }

    try
    {
      auto& section = conf.get_section("te_settings");
      auto server_ip = conf.get_value<std::string>(section, "ip_address");
      auto server_port = conf.get_value<std::uint32_t>(section, "port");

      _server_endpoint = ip::tcp::endpoint(ip::address::from_string(server_ip), server_port);

      _config_loaded = true;

      return ErrorCode::OK;
    }
    catch (const SystemException& ex)
    {
      Utils::Log.log("Target Environment Server: " + std::string(ex.what()));
      return ErrorCode::SystemError;
    }
    catch (const ConfigException& ex)
    {
      Utils::Log.log("Target Environment Server: " + std::string(ex.what()));
      return ErrorCode::ConfigError;
    }
  }

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

  ErrorCode Client::connect()
  {
    if (_connected == true)
    {
      return ErrorCode::OK;
    }

    if (_config_loaded == false)
    {
      Utils::Log.log("Target Environment client: Config not loaded.");
      return ErrorCode::SystemError;
    }

    error_code ec;
    _socket.connect(_server_endpoint, ec);

    if (ec)
    {
      Utils::Log.log("Target Environment client: " + ec.message());
      return ErrorCode::SystemError;
    }
 
    streambuf buff;
    auto size = read_until(_socket, buff, "\n", ec);
    if (ec)
    {
      Utils::Log.log("Target Environment client: " + ec.message());
      return ErrorCode::SystemError;
    }

    std::string res(buffer_cast<const char*>(buff.data()), size);
    if (res == RegisterSuccessMsg)
    {
      _connected = true;
      Utils::Log.log("Target Environment client: Client connect success.");
      return ErrorCode::OK;
    }
    else if (res == RegisterFailMsg)
    {
      Utils::Log.log("Target Environment client: Register failed.");
      return ErrorCode::RegisterFail;
    }
    else
    {
      Utils::Log.log("Target Environment client: Unknow type of message or wrong reply message format.");
      return ErrorCode::SystemError;
    }
  }

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

  ErrorCode Client::disconnect()
  {
    return close_client();
  }

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

  ErrorCode Client::get_targets(TargetTable& tbl, const SearchArea& sa, const CSUtils::GCSPoint& radar_loc, const ClockType::time_point& t)
  {
    if (_connected == false)
    {
      Utils::Log.log("Target Environment client: Request error. Client do not connected to server.");
      return ErrorCode::RequestFail;
    }

    auto dur = std::chrono::time_point_cast<DurationType>(t).time_since_epoch().count();    

    error_code ec;
    RequestMsg req_msg(sa, radar_loc, dur);

    write(_socket, buffer(req_msg.msg), ec);

    if (ec)
    {
      Utils::Log.log("Target Environment client: " + ec.message());
      close_client();
      return ErrorCode::SystemError;
    }

    streambuf buff;
    auto len = read_until(_socket, buff, "\n", ec);

    if (ec)
    {
      Utils::Log.log("Target Environment client: " + ec.message());
      close_client();
      return ErrorCode::SystemError;
    }

    std::string res(buffer_cast<const char*>(buff.data()), len);

    if (res == RequestFailMsg)
    {
      Utils::Log.log("Target Environment client: Request failed.");
      return ErrorCode::RequestFail;
    }
    
    tbl.clear();

    return deserialize(res, tbl);
  }

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

  ErrorCode Client::deserialize(const std::string& msg, TargetTable& tbl)
  {
    if (msg.empty())
    {
      Utils::Log.log("Target Environment client: Target list message is empty.");
      return ErrorCode::RequestFail;
    }

    rapidjson::Document root;
    root.Parse(msg.c_str());

    if (root.HasParseError())
    {
      Utils::Log.log("Target Environment client: Error parsing Target list message. Wrong data format.");
      return ErrorCode::RequestFail;
    }

    uint32_t ntargets;
    std::string msg_headser;

    try
    {
      msg_headser = get_value<std::string>(root, "message");
      ntargets = get_value<std::uint32_t>(root, "size");
    }
    catch (...)
    {
      Utils::Log.log("Target Environment client: Error parsing Target list message. Wrong data format.");
      return ErrorCode::RequestFail;
    }

    if (msg_headser != "TE_TargetsList")
    {
      Utils::Log.log("Target Environment client: Unknown type of message or wrong data format.");
      return ErrorCode::RequestFail;
    }

    if (ntargets == 0)
    {
      Utils::Log.log("Target Environment client: Targets not found.");
      return ErrorCode::OK;
    }

    if (root.HasMember("targets"))
    {
      const auto& targets_array = root["targets"];

      if (targets_array.IsArray() && targets_array.Size() == ntargets)
      {
        for (rapidjson::SizeType i = 0; i < targets_array.Size(); i++)
        {
          const auto& target = targets_array[i];
          
          if (target.IsObject())
          {
            Target item;
            item.target_id = get_value<std::uint32_t>(target, "id");
            item.x = get_array<double, 6>(target["X"]);
            item.y = get_array<double, 6>(target["Y"]);
            item.z = get_array<double, 6>(target["Z"]);
            tbl.add_target(item);
          }
          else
          {
            return ErrorCode::RequestFail;
            Utils::Log.log("Target Environment client: Unknown type of message or wrong data format.");
          }
        }
      }
      else
      {
        Utils::Log.log("Target Environment client: Unknown type of message or wrong data format.");
        return ErrorCode::RequestFail;
      }
    }
    else
    {
      Utils::Log.log("Target Environment client: Unknown type of message or wrong data format.");
      return ErrorCode::RequestFail;
    }

    return ErrorCode::OK;
  }
}
