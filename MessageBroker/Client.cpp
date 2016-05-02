#include "Client.h"

#include <cassert>
#include <chrono>
#include <thread>
#include <iostream>
#include <cstring>

#include "Message.h"
#include "HeartbeatMessage.h"

using namespace std;

MessageBroker::Client::Client() : 
  _config_loaded(false), 
  _heartbeat_start(false), 
  _connected(false), 
  _message_processing(false) 
{}


/**
* @fn  void Client::load_config(const ConfigManager& mng);
*
* @brief Loads a configuration using config manager.
*
* @author  Gorgool
* @date  26-Mar-16
*
* @exception ConfigException Throws if there are errors in config.
*
* @param mng Configuration manager object.
*
*/

void MessageBroker::Client::load_config(const ConfigManager& mng)
{
  try
  {
    auto& section = mng.get_section("message_broker_settings");
    _server_ip_address = mng.get_value<std::string>(section, "server_ip");
    _client_sub_port = mng.get_value<std::string>(section, "client_sub_port");
    _client_pub_port = mng.get_value<std::string>(section, "client_pub_port");
    _buffer_size = mng.get_value<std::uint32_t>(section, "buffer_size");
    _poll_interval = std::chrono::milliseconds(mng.get_value<std::uint32_t>(section, "poll_interval"));
    _heartbeat_interval = std::chrono::milliseconds(mng.get_value<std::uint32_t>(section, "heartbeat_interval"));
    _config_loaded = true;
  }
  catch (const std::exception& ex)
  {
    throw ConfigException("Message broker client : " + std::string(ex.what()));
  }

}

/**
* @fn  void Client::connect();
*
* @brief Connects to the server.
*
* @author  Gorgool
* @date  26-Mar-16
*
* @exception ConfigException Throws if config not loaded yet.
* @exception SystemException Throws if there are errors in socket or context creation.
*
*/

void MessageBroker::Client::connect()
{
  if (_config_loaded == false)
  {
    throw ConfigException("Message Broker client : Can't connect to the server. Config not loaded.");
  }

  try
  {
    _ctx = std::make_unique<zmq::context_t>();

    _output = std::make_unique<zmq::socket_t>(*_ctx, zmq::socket_type::pub);
    _output->connect("tcp://" + _server_ip_address + ":" + _client_pub_port);

    _input = std::make_unique<zmq::socket_t>(*_ctx, zmq::socket_type::sub);
    _input->connect("tcp://" + _server_ip_address + ":" + _client_sub_port);

    _connected = true;
  }
  catch (const zmq::error_t& ex)
  {
    _output.reset();
    _ctx.reset();
    _input.reset();
    throw SystemException("Message Broker client : Can't create MessageBroker. " + std::string(ex.what()));
  }
}

/**
* @fn  void Client::disconnect();
*
* @brief Disconnects from the server.
*
* @author  Gorgool
* @date  26-Mar-16
*
*/

void MessageBroker::Client::disconnect() noexcept
{
  if (_connected == false || _message_processing == true)
  {
    return;
  }
  _ctx.reset();
  _input.reset();
  _output.reset();
}

/**
* @fn  void Client::register_handler(std::string type, std::function<void(const char* msg, const std::size_t len)> handler);
*
* @brief Registers the handler for the specific type of message.
*
* @author  Gorgool
* @date  26-Mar-16
*
* @param type    Message type.
* @param handler Callback handler function.
*
*/

void MessageBroker::Client::register_handler(std::string type, std::function<void(const char* msg, const std::size_t len)> handler)
{
  if (_message_processing == true)
    throw ConfigException("Message Broker client: Can't register handler, message queue in process now.");

  // If handler for given type already exist ignore new handler
  if (_handlers_tbl.find(type) != _handlers_tbl.end())
    return;

  _handlers_tbl[type] = handler;
}

/**
* @fn  void Client::start_heartbeat();
*
* @brief Starts a heartbeat. Non blocking.
*
* @author  Gorgool
* @date  26-Mar-16
*
* @exception ConfigException
*
* @param id  The id of this instance
*
*/

void MessageBroker::Client::start_heartbeat(const std::string& id)
{
  if (_config_loaded == false)
    throw ConfigException("Message broker client : Can't start heartbeat loop. Config not loaded.");

  if (_connected == false)
    throw ConfigException("Message broker client : Can't start heartbeat loop. Not connected to server.");


  std::thread heartbeat_thread([this,&id]()
  {
    auto hb_message = Message("heartbeat", make_heartbeat_message(id));
    while (_heartbeat_start)
    {
      try
      {
        send_message(hb_message);
      }
      catch (const std::exception& ex)
      {
        std::cerr << "Message Broker Client: Heartbeat error. " << ex.what();
        _heartbeat_start = false;
        return;
      }
      this_thread::sleep_for(_heartbeat_interval);
    }
  });

  heartbeat_thread.detach();

  _heartbeat_start = true;
}

/**
* @fn  void Client::stop_heartbeat();
*
* @brief Stops a heartbeat.
*
* @author  Gorgool
* @date  26-Mar-16
*
*/

void MessageBroker::Client::stop_heartbeat() noexcept
{
  _heartbeat_start = false;
}

/**
* @fn  void Client::start_message_processing();
*
* @brief Starts message processing polling.
*
* @author  Gorgool
* @date  26-Mar-16
*
* @exception ConfigException
* @exception SystemException
*/

void MessageBroker::Client::start_message_processing()
{
  if (_message_processing == true)
    throw ConfigException("Message Broker client: Can't start message processing, already running.");

  if (_connected == false)
    throw ConfigException("Message Broker client: Can't start message processing, Message Broker not connected.");

  if (_handlers_tbl.empty() == true)
    throw ConfigException("Message Broker client: Can't start message processing, handlers table is empty.");

  // Subscribe for every message type in handlers table
  for (auto& type : _handlers_tbl)
  {
    _input->setsockopt(ZMQ_SUBSCRIBE, type.first.c_str(), type.first.size());
  }

  _message_processing = true;

  std::vector<zmq::pollitem_t> poll_vector;
  poll_vector.push_back(zmq::pollitem_t{ static_cast<void *>(*_input), 0, ZMQ_POLLIN, 0 });

  char* message_buffer = new char[_buffer_size];

  while (_message_processing)
  {
    try
    {
      int n_events = zmq::poll(poll_vector, _poll_interval);

      if (n_events == 0)
        continue;

      for (auto& item : poll_vector)
      {
        if (item.revents & ZMQ_POLLIN)
        {
          //  Read message
          auto message_size = zmq_recv(item.socket, message_buffer, _buffer_size, 0);

          if (message_size == -1)
          {
            delete[] message_buffer;
            if (zmq_errno() == ETERM)
              return;
            else
              throw SystemException("Message Broker client: Error while reading form socket.");
          }

          auto msg_type_end = strstr(message_buffer, " ");
          auto address = std::string(message_buffer, msg_type_end);
          auto offset = std::distance(message_buffer, msg_type_end) + 1;

          if (message_size > 0)
          {
            if (_handlers_tbl.find(address) == _handlers_tbl.end())
              throw ConfigException("Message Broker client : Handle for " + address + " not registered.");
            else
              _handlers_tbl[address](message_buffer + offset, message_size - offset);
          }
          else
            continue;
        }
        else
        {
          delete[] message_buffer;
          if (item.revents & ZMQ_POLLERR)
          {
            if (zmq_errno() == ETERM)
              return;
          }
          else
          {
            throw SystemException("Message Broker: Error while polling input.");
          }
        }
      }
    }
    catch (const zmq::error_t& ex)
    {
      delete[] message_buffer;
      throw SystemException("Message Broker: Error while polling input. " + std::string(ex.what()));
    }
  }
  delete[] message_buffer;
}

/**
* @fn  void Client::stop_message_processing();
*
* @brief Stops message processing.
*
* @author  Gorgool
* @date  26-Mar-16
*
*/

void MessageBroker::Client::stop_message_processing() noexcept
{
  _message_processing = false;
}
