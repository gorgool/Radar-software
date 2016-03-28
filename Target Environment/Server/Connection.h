#pragma once

#include <boost/asio.hpp>

namespace TargetEnvironment
{
  /*
    Connection to manage I/O buffers.
  */
  struct Connection
  {
    std::shared_ptr<boost::asio::streambuf> read_buffer;
    std::shared_ptr<boost::asio::streambuf> write_buffer;
    std::shared_ptr<boost::asio::ip::tcp::socket> socket;

    Connection(std::shared_ptr<boost::asio::ip::tcp::socket>);

    Connection();

    // Close connection
    void shutdown();

    ~Connection();
  };
}
