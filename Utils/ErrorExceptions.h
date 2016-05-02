#pragma once

#include <exception>

/**
 * @class SystemException
 *
 * @brief Exceptions caused by errors in the operating system functionality or 3rd party libraries.
 *
 * @author  Gorgool
 * @date  29.03.2016
 */

class SystemException : public std::exception
{
  std::string err_message;  
public:
  SystemException(const char* msg) : err_message(msg) {}
  SystemException(const std::string& msg) : err_message(msg) {}
  const char* what() const noexcept { return err_message.c_str(); }
};

/**
 * @class ConfigException
 *
 * @brief Exception while parsing configuration description.
 *
 * @author  Gorgool
 * @date  29.03.2016
 */

class ConfigException : public std::exception
{
  std::string err_message;  
public:
  ConfigException(const char* msg) : err_message(msg) {}
  ConfigException(const std::string& msg) : err_message(msg) {}
  const char* what() const noexcept { return err_message.c_str(); }
};
