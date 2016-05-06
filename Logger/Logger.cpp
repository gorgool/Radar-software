#include "Logger.h"
#include <chrono>
#include <ctime>
#include <random>
#include <cstring>
#include <sstream>
#include <boost/filesystem.hpp>
#include <clocale>

using namespace boost;

/** @brief The output chanel set to std::cerr, by default.*/
std::shared_ptr<spdlog::logger> Logger::_display_chanel = spdlog::stdout_logger_mt("console");

/** @brief The output file channel. */
std::shared_ptr<spdlog::logger> Logger::_file_chanel;

/** @brief The path to location with log files. */
filesystem::path Logger::_path;

/** @brief Currently opened session id. */
std::uint32_t Logger::_id;

/** @brief true if configuration loaded. */
bool Logger::_config_loaded = false;

/** @brief true if there is opened session. */
bool Logger::_session_opened = false;


/**
* @fn:  ~Logger();
*
* @brief Destructor
* 
* @author Gorgool
* @date 28.04.2016
*
*/

Logger::~Logger()
{
  close_session();
}

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

void Logger::load_config(const ConfigManager& mng)
{
  try
  {
    auto& section = mng.get_section("logger");

    _path = filesystem::path(mng.get_value<std::string>(section, "log_dir"));

    if (!filesystem::exists(_path))
      filesystem::create_directory(_path);

    spdlog::set_async_mode(std::pow(2, 20));
    spdlog::set_pattern("[%l - %d.%m.%Y %H:%M:%S.%F] %v");

    _display_chanel->set_level(spdlog::level::notice);
    _display_chanel->set_level(spdlog::level::err);
    _display_chanel->set_level(spdlog::level::debug);

    _config_loaded = true;
  }
  catch (const spdlog::spdlog_ex& ex)
  {
    throw SystemException("Logger : " + std::string(ex.what()));
  }
  catch (...)
  {
    throw;
  }
}

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
*
* @param id  The identifier.
*/

void Logger::start_session(const std::uint32_t id)
{
  if (_config_loaded == false)
    throw ConfigException("Logger: Config not loaded.");

  auto check_id = [&](const std::uint32_t new_id) -> bool
  {
    filesystem::directory_iterator dir_it(_path), end_dir;
    auto new_id_str = std::to_string(new_id);
    while (dir_it != end_dir)
    {
      std::string name_str(dir_it->path().filename().generic_string());
      auto id_str = name_str.substr(0, name_str.find(" "));

      if (new_id_str == id_str)
        return true;

      dir_it++;
    }
    return false;
  };

  if (id == 0)
  {
    // Generate unique id
    std::random_device seed;
    std::mt19937 id_generator(seed());
    std::uniform_int<std::uint32_t> get_new_id(0, std::numeric_limits<std::uint32_t>::max());

    auto new_id = get_new_id(id_generator);
    std::size_t guard_counter = 0;
    while (check_id(new_id)) 
    {
      new_id = get_new_id(id_generator);
      guard_counter++;
      if (guard_counter == 1000)
        throw SystemException("Logger: Id generator guard overflow.");
    }

    _id = new_id;
  }
  else
  {
    if (check_id(id))
      throw ConfigException("Logger: Wrong id, log file with such id already exists.");

    _id = id;
  }
  
  auto current_date = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  char buff[26];
  ctime_s(buff, 26, &current_date);

  auto temp = std::string(buff);
  auto time_str = temp.substr(0, temp.find('\n'));

  auto pos = time_str.find(':');
  while (pos != std::string::npos)
  {
    time_str.replace(pos, 1, 1, '-');
    pos = time_str.find(':', pos);
  }

  std::string filename = std::to_string(_id) + " - " + time_str + ".log";

  try
  {
    _file_chanel = spdlog::rotating_logger_mt("file_logger", _path.generic_string() + filename, 1024 * 1024 * 5, 10);;
    _file_chanel->set_level(spdlog::level::notice);
    _file_chanel->set_level(spdlog::level::err);
    _file_chanel->set_level(spdlog::level::debug);
  }
  catch (const spdlog::spdlog_ex& ex)
  {
    throw SystemException("Logger : " + std::string(ex.what()));
  }

  _session_opened = true;
}

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

uint32_t Logger::get_session_id() noexcept
{
  return _id;
}

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

void Logger::close_session()
{
  try
  {
    spdlog::drop_all();
    _file_chanel.reset();
    _session_opened = false;
  }
  catch (const spdlog::spdlog_ex& ex)
  {
    throw SystemException("Logger : " + std::string(ex.what()));
  }
  catch (...)
  {
    throw SystemException("Logger : Unknown error in close_session().");
  }
}

/**
* @fn  log_file(SeverityLevel level, const std::string& sender, const std::string& log_message);
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

void Logger::log_display(SeverityLevel level, const std::string & sender, const std::string & log_message)
{
  if (_session_opened)
  {
    try
    {
      switch (level)
      {
      case Error:
        _display_chanel->error() << sender << " : " << log_message;
        break;
      case Notice:
        _display_chanel->notice() << sender << " : " << log_message;
        break;
      case Debug:
        _display_chanel->debug() << sender << " : " << log_message;
        break;
      default:
        break;
      }
    }
    catch (const spdlog::spdlog_ex& ex)
    {
      throw SystemException("Logger : " + std::string(ex.what()));
    }
  }
}

/**
* @fn  log_file(SeverityLevel level, const std::string& sender, const std::string& log_message);
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

void Logger::log_file(SeverityLevel level, const std::string & sender, const std::string & log_message)
{
  if (_session_opened)
  {
    try
    {
      switch (level)
      {
      case Error:
        _file_chanel->error() << sender << " : " << log_message;
        break;
      case Notice:
        _file_chanel->notice() << sender << " : " << log_message;
        break;
      case Debug:
        _file_chanel->debug() << sender << " : " << log_message;
        break;
      default:
        break;
      }
    }
    catch (const spdlog::spdlog_ex& ex)
    {
      throw SystemException("Logger : " + std::string(ex.what()));
    }
  }
}

/**
* @fn  log_dispfile(SeverityLevel level, const std::string& sender, const std::string& log_message);
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

void Logger::log_dispfile(SeverityLevel level, const std::string & sender, const std::string & log_message)
{
  if (_session_opened)
  {
    try
    {
      switch (level)
      {
      case Error:
        _file_chanel->error() << sender << " : " << log_message;
        _display_chanel->error() << sender << " : " << log_message;
        break;
      case Notice:
        _file_chanel->notice() << sender << " : " << log_message;
        _display_chanel->notice() << sender << " : " << log_message;
        break;
      case Debug:
        _file_chanel->debug() << sender << " : " << log_message;
        _display_chanel->debug() << sender << " : " << log_message;
        break;
      default:
        break;
      }
    }
    catch (const spdlog::spdlog_ex& ex)
    {
      throw SystemException("Logger : " + std::string(ex.what()));
    }
  }
}



