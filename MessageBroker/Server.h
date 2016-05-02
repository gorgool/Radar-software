#pragma once
#include <string>
#include <memory>
#include "Utils/ErrorExceptions.h"
#include "ConfigManager/ConfigManager.hpp"
#include <zmq.hpp>

namespace MessageBroker
{
  class Server
  {
    /** @brief The port for outgoing messages. */
    std::string _server_pub_port;

    /** @brief The port for incoming messages. */
    std::string _server_sub_port;

    /** @brief ZeroMQ context. */
    std::unique_ptr<zmq::context_t> _ctx;

    /** @brief The output socket. */
    std::unique_ptr<zmq::socket_t> _output;

    /** @brief The input socket. */
    std::unique_ptr<zmq::socket_t> _input;

    /** @brief The control socket. */
    std::unique_ptr<zmq::socket_t> _control;

  public:

    /**
    * @fn: void load_config();
    *
    * @brief�Loads the config
    *�
    * @author Gorgool
    * @date 28/04/2016
    *
    * @exception�ConfigException
    *
    */

    void load_config(const ConfigManager& mng);

    /**
    * @fn: void start();
    *
    * @brief�Starts server. Blocking.
    *�
    * @author Gorgool
    * @date 28/04/2016
    *
    * @exception�SystemException
    *
    */

    void start();

    /**
    * @fn: void stop() noexcept;
    *
    * @brief�Stops server.
    *�
    * @author Gorgool
    * @date 28/04/2016
    *
    * @exception�SystemException
    *
    */

    void stop();
  };
}
