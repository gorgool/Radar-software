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
public:
  SystemException(const char* msg) : exception(msg) {}
  SystemException(const std::string& msg) : exception(msg.c_str()) {}
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
public:
  ConfigException(const char* msg) : exception(msg) {}
  ConfigException(const std::string& msg) : exception(msg.c_str()) {}
};