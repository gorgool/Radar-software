#include "../include/Connection.h"
#include <random>

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

  void TargetEnvironment::Connection::shutdown()
  {
    if (socket.get()->is_open())
    {
      boost::system::error_code ec;
      socket.get()->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
      if (ec)
      {
        DFLOG("Connection: " + ec.message());
      }

      socket.get()->close(ec);
      if (ec)
      {
        DFLOG("Connection: " + ec.message());
      }
    }
  }

  Connection::~Connection() {}
}
