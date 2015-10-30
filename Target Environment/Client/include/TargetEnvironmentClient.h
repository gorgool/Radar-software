#pragma once

#include <boost/asio.hpp>
#include <chrono>
#include "../../Shared/include/TargetTable.h"
#include <unordered_map>
#include "../../Shared/include/ReferencePointDesc.h"
#include "../../Shared/include/TargetEnvironmentErrors.h"
#include "../../Shared/include/TargetEnvironmentMessages.h"

#define DEBUG_LOG
#include "../../../Utils/include/Logger.h"

namespace TargetEnvironment
{
  class TargetEnvironmentClient
  {
    boost::asio::io_service _service;
    boost::asio::ip::tcp::socket _socket;

    ReferencePointDesc _reference_point;

    bool parse_targets(const char*, const std::size_t);
    ErrorCode close_client();

    const std::size_t MAX_TARGETS = 20000;
    TargetTable _tbl;

    bool _connected;

  public:
    TargetEnvironmentClient();
    ~TargetEnvironmentClient();

    /*
        Connect to server. Return :
        - OK_EC if succeeded or was established earlier
        - RegisterFail_EC if server cant register client (wrong id)
    */
    ErrorCode connect(const boost::asio::ip::tcp::endpoint&);

    /*
        Return true if connection established.
    */
    bool is_connected();

    /*
        Disconnect from server
    */
    ErrorCode disconnect();

    /*
        Request targets list (by time t) from server. Return:
        - OK_EC if request procced succefully
        - RequestFail_EC if request failed
    */
    ErrorCode get_targets(std::unordered_map<std::size_t, TargetDesc>&, const TimeType& time);
  };
}
