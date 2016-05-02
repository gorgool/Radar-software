#include "Server.h"
#include <iostream>

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

void MessageBroker::Server::load_config(const ConfigManager& mng)
{
  try
  {
    auto& section = mng.get_section("message_broker_settings");
    _server_pub_port = mng.get_value<std::string>(section, "server_pub_port");
    _server_sub_port = mng.get_value<std::string>(section, "server_sub_port");
  }
  catch (const std::exception& ex)
  {
    throw ConfigException("Message Broker server : " + std::string(ex.what()));
  }

}

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

void MessageBroker::Server::start()
{
  try
  {
    _ctx = std::make_unique<zmq::context_t>();

    _output = std::make_unique<zmq::socket_t>(*_ctx, zmq::socket_type::xpub);
    _output->bind("tcp://*:" + _server_pub_port);

    _input = std::make_unique<zmq::socket_t>(*_ctx, zmq::socket_type::xsub);
    _input->bind("tcp://*:" + _server_sub_port);

    _control = std::make_unique<zmq::socket_t>(*_ctx, zmq::socket_type::rep);
    _control->connect("tcp://127.0.0.1:6666");

    zmq::proxy_steerable(static_cast<void *>(*_input), static_cast<void *>(*_output), nullptr, static_cast<void *>(*_control));
  }
  catch (const std::exception& ex)
  {
    throw SystemException("Message broker server: Error. " + std::string(ex.what()));
  }
}

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

void MessageBroker::Server::stop()
{
  try
  {
    zmq::socket_t temp(*_ctx, zmq::socket_type::req);
    temp.bind("tcp://*:6666");
    temp.send("TERMINATE", 9);
  }
  catch (const std::exception& ex)
  {
    throw SystemException("Message broker server: Error. " + std::string(ex.what()));
  }
}
