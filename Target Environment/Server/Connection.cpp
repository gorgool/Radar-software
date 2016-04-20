#include "Connection.h"
#include <random>

#include <Utils/Logger.h>

namespace TargetEnvironment
{
  Connection::Connection(std::shared_ptr<boost::asio::ip::tcp::socket> s) :
    read_buffer(new boost::asio::streambuf()),
    write_buffer(new boost::asio::streambuf()),
    socket(s)
  {}

  Connection::Connection() :
    read_buffer(new boost::asio::streambuf()),
    write_buffer(new boost::asio::streambuf())
  {}

  /**
   * @fn  void TargetEnvironment::Connection::shutdown()
   *
   * @brief Shuts down this connection.
   *
   * @author  Gorgool
   * @date  20.04.2016
   */

  void TargetEnvironment::Connection::shutdown()
  {
    if (socket.get()->is_open())
    {
      boost::system::error_code ec;
      socket.get()->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
      if (ec)
      {
        Utils::Log.log("Connection: " + ec.message());
      }

      socket.get()->close(ec);
      if (ec)
      {
        Utils::Log.log("Connection: " + ec.message());
      }
    }
  }

  Connection::~Connection() {}
}
