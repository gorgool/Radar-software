#pragma once
#include "ConfigManager/ConfigManager.hpp"
#include <iostream>
#include <array>
#include <thread>
#include "Libraries/spdlog/spdlog.h"

/**
 * @enum  SeverityLevel
 *
 * @brief Values that represent severity levels.
 */

enum SeverityLevel
{
  ///< Error level of severity
  Error = 0,
  ///< Notice level of severity
  Notice = 1,
  ///< Debug level of severity
  Debug = 2
};

class Logger
{
  /** @brief The output display chanel, e.g std::cout, std::cerr etc. */
  static std::shared_ptr<spdlog::logger> _display_chanel;

  /** @brief The output file channel. */
  static std::shared_ptr<spdlog::logger> _file_chanel;

  /** @brief The path to location with log files. */
  static boost::filesystem::path _path;

  /** @brief Number of files in rotation. */
  static std::uint32_t _num_files;

  /** @brief true if configuration loaded. */
  static bool _config_loaded;

  /** @brief Currently opened session id. */
  static std::uint32_t _id;

  /** @brief true if there is opened session. */
  static bool _session_opened;

public:

  /**
  * @fn:  ~Logger();
  *
  * @brief Destructor
  *
  * @author Gorgool
  * @date 28.04.2016
  *
  */

  ~Logger();

  /**
   * @fn  void Logger::load_config(const ConfigManager& mng);
   *
   * @brief Loads a configuration.
   *
   * @author  Gorgool
   * @date  26.04.2016
   *
   * @exception ConfigException Thrown when a configuration error condition occurs.
   *
   * @param mng The config manager.
   */

  static void load_config(const ConfigManager& mng);

  /**
   * @fn  void Logger::start_session(const std::uint32_t id = 0);
   *
   * @brief Starts a session for logging.
   *        Open new file for logging. If called with id=0 (default) random id would be generated.
   *
   * @author  Gorgool
   * @date  26.04.2016
   *
   * @exception ConfigException Thrown when a session begins before configurations setup.
   * @exception SystemException Thrown when can't generate id.
   *
   * @param id  The identifier.
   */

  static void start_session(const std::uint32_t id = 0);

  /**
   * @fn  uint32_t Logger::get_session_id() noexcept;
   *
   * @brief Gets session identifier.
   *
   * @author  Gorgool
   * @date  26.04.2016
   *
   * @return  The session identifier.
   */

  static uint32_t get_session_id() noexcept;

  /**
   * @fn  void Logger::close_session() throw(SystemException);
   *
   * @brief Closes the logging session.
   *        Clears all inner buffer and closes open file.
   * 
   * @author  Gorgool
   * @date  26.04.2016
   *
   * @exception SystemException Thrown when a error occurs.
   */

  static void close_session();

  /**
   * @fn  log_display(SeverityLevel level, const std::string& sender, const std::string& log_message) noexcept;
   *
   * @brief Display a log message.
   *
   * @author  Gorgool
   * @date  26.04.2016
   *
   * @exception SystemException Thrown when a error occurs.
   *
   * @param level       The level of severity.
   * @param sender      The sender name.
   * @param log_message Log message.
   */

  static void log_display(SeverityLevel level, const std::string& sender, const std::string& log_message);

  /**
  * @fn  log_file(SeverityLevel level, const std::string& sender, const std::string& log_message) noexcept;
  *
  * @brief Write the log message in the file.
  *
  * @author  Gorgool
  * @date  26.04.2016
  *
  * @exception SystemException Thrown when a error occurs.
  *
  * @param level       The level of severity.
  * @param sender      The sender name.
  * @param log_message Log message.
  */

  static void log_file(SeverityLevel level, const std::string& sender, const std::string& log_message);

  /**
  * @fn  log_dispfile(SeverityLevel level, const std::string& sender, const std::string& log_message) noexcept;
  *
  * @brief Write the log message in the file and in the console.
  *
  * @author  Gorgool
  * @date  26.04.2016
  *
  * @exception SystemException Thrown when a error occurs.
  *
  * @param level       The level of severity.
  * @param sender      The sender name.
  * @param log_message Log message.
  */

  static void log_dispfile(SeverityLevel level, const std::string& sender, const std::string& log_message);
};
