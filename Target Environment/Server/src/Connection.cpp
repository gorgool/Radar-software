#include "../include/Connection.h"
#include <random>

namespace TargetEnvironment
{
  Connection::Connection(std::shared_ptr<boost::asio::ip::tcp::socket> s) :
    read_buffer(new boost::asio::streambuf()),
    write_buffer(new boost::asio::streambuf()),
    socket(s)
  {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<std::size_t> id;
    ref_point.client_id = id(gen);
  }

  Connection::Connection() :
    read_buffer(new boost::asio::streambuf()),
    write_buffer(new boost::asio::streambuf())
  {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<std::size_t> id;
    ref_point.client_id = id(gen);
  }

  void TargetEnvironment::Connection::shutdown()
  {
    if (socket.get()->is_open())
    {
      boost::system::error_code ec;
      socket.get()->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
      if (ec)
      {
        DLOG("Connection: " + ec.message());
      }

      socket.get()->close(ec);
      if (ec)
      {
        DLOG("Connection: " + ec.message());
      }
    }
  }

  Connection::~Connection() {}
}
