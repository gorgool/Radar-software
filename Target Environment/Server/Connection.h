#pragma once

#include <boost/asio.hpp>

namespace TargetEnvironment
{
  /**
   * @struct  Connection
   *
   * @brief A connection to manage I/O buffers.
   *
   * @author  Gorgool
   * @date  20.04.2016
   */

  struct Connection
  {
    /** @brief Buffer for read data. */
    std::shared_ptr<boost::asio::streambuf> read_buffer;
    /** @brief Buffer for write data. */
    std::shared_ptr<boost::asio::streambuf> write_buffer;
    /** @brief The socket. */
    std::shared_ptr<boost::asio::ip::tcp::socket> socket;

    Connection(std::shared_ptr<boost::asio::ip::tcp::socket>);

    Connection();

    /**
    * @fn  void TargetEnvironment::Connection::shutdown()
    *
    * @brief Shuts down this connection.
    *
    * @author  Gorgool
    * @date  20.04.2016
    */
    void shutdown();

    ~Connection();
  };
}
