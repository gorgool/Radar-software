#pragma once

#include <boost/asio.hpp>
#define DEBUG_LOG
#include "../../../Utils/include/Logger.h"
#include "../../Shared/include/ReferencePointDesc.h"

namespace TargetEnvironment
{
  struct Connection
  {
    ReferencePointDesc ref_point;
    std::shared_ptr<boost::asio::streambuf> read_buffer;
    std::shared_ptr<boost::asio::streambuf> write_buffer;
    std::shared_ptr<boost::asio::ip::tcp::socket> socket;

    Connection(std::shared_ptr<boost::asio::ip::tcp::socket>);
    Connection();
    void shutdown();
    ~Connection();
  };
}
