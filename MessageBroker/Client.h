#pragma once

#include <zmq.hpp>

#include <map>
#include <cinttypes>
#include <functional>
#include <memory>
#include "ConfigManager/ConfigManager.hpp"
#include "Utils/ErrorExceptions.h"

namespace MessageBroker
{
  class Client
  {
  public:

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

    void load_config(const ConfigManager& mng);

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

    void connect();

    /**
     * @fn  void Client::disconnect();
     *
     * @brief Disconnects from the server.
     *
     * @author  Gorgool
     * @date  26-Mar-16
     *
     */

    void disconnect() noexcept;

    /**
     * @fn  template<typename MessageType> void Client::send_message(const MessageType& message);
     *
     * @brief Sends (publish) a message.
     *
     * @author  Gorgool
     * @date  26-Mar-16
     *
     * @exception SystemException
     *
     * @tparam  MessageType Type of the message type. Must have serialaize() method.
     * @param message The message.
     *
     */

    template<typename MessageType>
    void send_message(const MessageType& message);

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

    void register_handler(std::string type, std::function<void(const char* msg, const std::size_t len)> handler);

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

    void start_heartbeat(const std::string& id);

    /**
     * @fn  void Client::stop_heartbeat();
     *
     * @brief Stops a heartbeat.
     *
     * @author  Gorgool
     * @date  26-Mar-16
     *
     */

    void stop_heartbeat() noexcept;

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

    void start_message_processing();

    /**
     * @fn  void Client::stop_message_processing();
     *
     * @brief Stops message processing.
     *
     * @author  Gorgool
     * @date  26-Mar-16
     *
     */

    void stop_message_processing() noexcept;

  private:

    /** @brief The ZeroMq context. */
    std::unique_ptr<zmq::context_t> _ctx;

    /** @brief The output publish socket. */
    std::unique_ptr<zmq::socket_t> _output;

    /** @brief The input subscriber socket. */
    std::unique_ptr<zmq::socket_t> _input;

    /** @brief The handlers table. */
    std::map<std::string, std::function<void(const char* msg, const std::size_t len)>> _handlers_tbl;

    /** @brief The server IP address. */
    std::string _server_ip_address;

    /** @brief The server port for incoming messages. */
    std::string _client_sub_port;

    /** @brief The server port for outgoing messages. */
    std::string _client_pub_port;

    /** @brief The heartbeat interval in milliseconds. */
    std::chrono::milliseconds _heartbeat_interval;

    /** @brief Size of input messages buffer in bytes. */
    std::uint32_t _buffer_size;

    /** @brief Interval in messages poll function in milliseconds. */
    std::chrono::milliseconds _poll_interval;

    // Flags
    /** @brief true if configuration loaded. */
    bool _config_loaded;

    /** @brief true to heartbeat start. */
    bool _heartbeat_start;

    /** @brief true if connected. */
    bool _connected;

    /** @brief true to message processing. */
    bool _message_processing;
  };

  // Send message implementation.
  template<typename MessageType>
  inline void Client::send_message(const MessageType & message)
  {
    if (_config_loaded == false)
      throw ConfigException("Message Broker client: Can't sent message. Config not loaded.");

    if (_connected == false)
      throw ConfigException("Message Broker client: Can't sent message. Not connected to the server.");


    std::string serialized_representation = message.serialize();
    try
    {
      _output->send(serialized_representation.c_str(), serialized_representation.size());
    }
    catch (const std::exception& ex)
    {
      throw SystemException("Message broker client: Can't sent message." + std::string(ex.what()));
    }
  }
}
