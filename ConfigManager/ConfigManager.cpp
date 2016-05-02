#include "ConfigManager.hpp"
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>
#include <sstream>
#include <cstring>
#include <iostream>
#include <fstream>
#include <cassert>

using namespace boost;

/**
* @fn  void ConfigManager::set_path(const std::string& path);
*
* @brief Sets a path to directory with configuration files.
*
* @author  Gorgool
* @date  29.03.2016
*
* @exception SystemException Thrown if the path don't exists.
*
* @param path  Full pathname of the file.
*/

void ConfigManager::set_path(const std::string & path)
{
  try
  {
    _path = filesystem::path(path);
    if (filesystem::exists(_path))
    {
      if (!filesystem::is_directory(_path))
        throw SystemException("Configuration Manager: Path is not a directory.");
    }
    else
      throw SystemException("Configuration Manager: Directory don't exist.");
  }
  catch (filesystem::filesystem_error& ex)
  {
    throw SystemException("Configuration Manager: " + std::string(ex.what()));
  }
}
/**
* @fn  void ConfigManager::load_config(const std::string& config_name);
*
* @brief Loads a configuration by name.
*
* @author  Gorgool
* @date  29.03.2016
*
* @exception SystemException Thrown if the config file don't exists.
* @exception ConfigException Thrown if a parse error occurs.
*
* @param config_name Name of the configuration.
*/

void ConfigManager::load_config(const std::string & config_name)
{
  try
  {
    filesystem::path file_path = _path / filesystem::path(config_name + ".cfg");

    if (!filesystem::exists(file_path))
      throw SystemException("Configuration Manager: Configuration file don't exist.");
 
    std::ifstream config_file(file_path.generic_string());

    if (config_file.good() == false)
      throw SystemException("Configuration Manager: Con't open configuration file.");

    std::stringstream config_stream;
    config_stream << config_file.rdbuf();

    config_file.close();

    rapidjson::Document temp;
    temp.Parse(config_stream.str().c_str());

    if (temp.HasParseError() || !temp.IsObject())
      throw ConfigException("Configuration Manager: Configuration file parse error.");

    _json_config = std::move(temp);
    _config_name = config_name;

    _config_loaded = true;
  }
  catch (filesystem::filesystem_error& ex)
  {
    throw SystemException("Configuration Manager: " + std::string(ex.what()));
  }
  catch (...)
  {
    throw;
  }
}

/**
* @fn  void ConfigManager::save_config(const std::string& config_name) const;
*
* @brief Saves configuration in a file.
*
* @author  Gorgool
* @date  29.03.2016
*
* @exception SystemException Thrown if can't create file or write in a file.
*
* @param config_name Name of the configuration.
*/

void ConfigManager::save_config(const std::string & config_name) const
{
  if (_config_loaded == false)
    throw ConfigException("Configuration Manager: Config not loaded.");

  rapidjson::StringBuffer buffer;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
  _json_config.Accept(writer);
  const std::string save_str = buffer.GetString();

  try
  {
    std::ofstream new_config(_path.generic_string() + std::string(config_name + ".cfg"));
    new_config << save_str;
    new_config.close();
  }
  catch (const std::exception& ex)
  {
    throw SystemException("Configuration Manager: " + std::string(ex.what()));
  }
}

/**
* @fn  void ConfigManager::update_config(const std::string& config_str);
*
* @brief Updates the configuration described by JSON config_str. All unsaved changes will be lost.
*
* @author  Gorgool
* @date  29.03.2016
*
* @exception ConfigException Thrown if a parse error occurs.
*
* @param config_str  The configuration JSON string.
*/

void ConfigManager::update_config(const std::string & config_str)
{
  if (_config_loaded == false)
    throw ConfigException("Configuration Manager: Config not loaded.");

  rapidjson::Document temp;
  temp.Parse(config_str.c_str());

  if (temp.HasParseError() || !temp.IsObject())
    throw ConfigException("Configuration Manager: Configuration update error.");

  _json_config = std::move(temp);
}

/**
* @fn  std::string ConfigManager::config_name() const noexcept;
*
* @brief Current configuration name.
*
* @author  Gorgool
* @date  29.03.2016
*
* @return  configuration name.
*/

std::string ConfigManager::config_name() const noexcept
{
  if (_config_loaded == false)
    return std::string();

  return _config_name;
}

/**
* @fn  std::string ConfigManager::config_path() const noexcept;
*
* @brief Current configuration path.
*
* @author  Gorgool
* @date  29.03.2016
*
* @return  configuration path.
*/

std::string ConfigManager::config_path() const noexcept
{
  if (_config_loaded == false)
    return std::string();

  return _path.generic_string();
}

/**
* @fn  std::string ConfigManager::get_config_str() const noexcept;
*
* @brief Gets configuration JOSN string.
*
* @author  Gorgool
* @date  29.03.2016
*
* @return  The configuration JSON string.
*/

std::string ConfigManager::get_config_str() const noexcept
{
  if (_config_loaded == false)
    return std::string();

  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  _json_config.Accept(writer);
  
  return buffer.GetString();
}

/**
* @fn  Settings ConfigManager::get_section(const std::string& section_name) const;
*
* @brief Gets the settings for specific section.
*
* @author  Gorgool
* @date  29.03.2016
*
* @exception ConfigException Thrown if a section don't exists.
*
* @param section_name  Name of the section.
*
* @return  The section settings.
*/

ConfigManager::Settings ConfigManager::get_section(const std::string & section_name) const
{
  if (_config_loaded == false)
    throw ConfigException("Configuration Manager: Config not loaded.");

  if (_json_config.HasMember(section_name.c_str()))
  {
    if (_json_config[section_name.c_str()].IsObject())
    {
      return _json_config[section_name.c_str()];
    }
    else
      throw ConfigException("Configuration Manager: Unknown config section name.");
  }
  else
    throw ConfigException("Configuration Manager: Unknown config section name.");
}

/**
* @fn  Object ConfigManager::get_object(const Settings& settings, const char* key) const;
*
* @brief Gets an JSON object by key.
*
* @author  Gorgool
* @date  29.03.2016
*
* @exception ConfigException Thrown if the key don't exists or value by key not an array or an object.
*
* @param settings  Section settings.
* @param key       The key.
*
* @return  The JSON object (array or object).
*/

ConfigManager::Object ConfigManager::get_object(const Settings settings, const char * key) const
{
  if (_config_loaded == false)
    throw ConfigException("Configuration Manager: Config not loaded.");

  if (settings.HasMember(key))
  {
    if (settings[key].IsObject() || settings[key].IsArray())
    {
      return settings[key];
    }
    else
      throw ConfigException("Configuration Manager: Not an object.");
  }
  else
    throw ConfigException("Configuration Manager: Unknown object name.");
}

/**
* @fn  void ConfigManager::set_value(const Settings& settings, const char* key, const ValueType& value);
*
* @brief Sets a value. Overloaded for different values. Not template.
*
* @author  Gorgool
* @date  29.03.2016
*
* @exception ConfigException Thrown if value by key don't exists.
*
* @tparam  ValueType Value type parameter.
* @param settings  Section settings or object.
* @param key       The key.
* @param value     The value.
*/

void ConfigManager::set_value(Settings settings, const char * key, const std::string & value)
{
  if (_config_loaded == false)
    throw ConfigException("Configuration Manager: Config not loaded.");

  if (key != nullptr)
  {
    if (settings.HasMember(key) && settings[key].IsString())
    {
      auto& obj = const_cast<rapidjson::Value&>(settings[key]);
      obj.SetString(value.c_str(), value.size(), _json_config.GetAllocator());
    }
    else
      throw ConfigException("Configuration Manager: Parse error.");
  }
  else
  {
    throw ConfigException("Configuration Manager: Parse error.");
  }
}

void ConfigManager::set_value(Settings settings, const char * key, const double value)
{
  if (_config_loaded == false)
    throw ConfigException("Configuration Manager: Config not loaded.");

  if (key != nullptr)
  {
    if (settings.HasMember(key) && settings[key].IsDouble())
    {
      auto& obj = const_cast<rapidjson::Value&>(settings[key]);
      obj.SetDouble(value);
    }
    else
      throw ConfigException("Configuration Manager: Parse error.");
  }
  else
  {
    throw ConfigException("Configuration Manager: Parse error.");
  }
}

void ConfigManager::set_value(Settings settings, const char * key, const std::uint32_t value)
{
  if (_config_loaded == false)
    throw ConfigException("Configuration Manager: Config not loaded.");

  if (key != nullptr)
  {
    if (settings.HasMember(key) && settings[key].IsUint())
    {
      auto& obj = const_cast<rapidjson::Value&>(settings[key]);
      obj.SetUint(value);
    }
    else
      throw ConfigException("Configuration Manager: Parse error.");
  }
  else
  {
    throw ConfigException("Configuration Manager: Parse error.");
  }
}

void ConfigManager::set_value(Settings settings, const char * key, const std::uint64_t value)
{
  if (_config_loaded == false)
    throw ConfigException("Configuration Manager: Config not loaded.");

  if (key != nullptr)
  {
    if (settings.HasMember(key) && settings[key].IsUint64())
    {
      auto& obj = const_cast<rapidjson::Value&>(settings[key]);
      obj.SetUint64(value);
    }
    else
      throw ConfigException("Configuration Manager: Parse error.");
  }
  else
  {
    throw ConfigException("Configuration Manager: Parse error.");
  }
}

void ConfigManager::set_value(Settings settings, const char * key, const std::int32_t value)
{
  if (_config_loaded == false)
    throw ConfigException("Configuration Manager: Config not loaded.");

  if (key != nullptr)
  {
    if (settings.HasMember(key) && settings[key].IsInt())
    {
      auto& obj = const_cast<rapidjson::Value&>(settings[key]);
      obj.SetInt(value);
    }
    else
      throw ConfigException("Configuration Manager: Parse error.");
  }
  else
  {
    throw ConfigException("Configuration Manager: Parse error.");
  }
}

void ConfigManager::set_value(Settings settings, const char * key, const std::int64_t value)
{
  if (_config_loaded == false)
    throw ConfigException("Configuration Manager: Config not loaded.");

  if (key != nullptr)
  {
    if (settings.HasMember(key) && settings[key].IsInt64())
    {
      auto& obj = const_cast<rapidjson::Value&>(settings[key]);
      obj.SetInt64(value);
    }
    else
      throw ConfigException("Configuration Manager: Parse error.");
  }
  else
  {
    throw ConfigException("Configuration Manager: Parse error.");
  }
}

template<typename RetType>
RetType ConfigManager::get_value(const Settings settings, const char * key) const
{
  // Not implemented for generic type
  assert(false);
  return RetType();
}

template<>
std::string ConfigManager::get_value<std::string>(const Settings settings, const char* key) const
{
  if (key != nullptr)
  {
    if (settings.HasMember(key) && settings[key].IsString())
    {
      return settings[key].GetString();
    }
    else
      throw ConfigException("Configuration Manager: Parse error.");
  }
  else
  {
    if (settings.IsString())
      return settings.GetString();
    else
      throw ConfigException("Configuration Manager: Parse error.");
  }
}

template<>
double ConfigManager::get_value<double>(const Settings settings, const char* key) const
{
  if (key != nullptr)
  {
    if (settings[key].IsNumber() && settings.HasMember(key))
      return settings[key].GetDouble();
    else
      throw ConfigException("Configuration Manager: Parse error.");
  }
  else
  {
    if (settings.IsNumber())
      return settings.GetDouble();
    else
      throw ConfigException("Configuration Manager: Parse error.");
  }
}

template<>
std::uint32_t ConfigManager::get_value<std::uint32_t>(const Settings settings, const char* key) const
{
  if (key != nullptr)
  {
    if (settings.HasMember(key) && settings[key].IsUint())
    {
      return settings[key].GetUint();
    }
    else
      throw ConfigException("Configuration Manager: Parse error.");
  }
  else
  {
    if (settings.IsUint())
    {
      return settings.GetUint();
    }
    else
      throw ConfigException("Configuration Manager: Parse error.");
  }
}

template<>
std::int32_t ConfigManager::get_value<std::int32_t>(const Settings settings, const char* key) const
{
  if (key != nullptr)
  {
    if (settings.HasMember(key) && settings[key].IsInt())
      return settings[key].GetInt();
    else
      throw ConfigException("Configuration Manager: Parse error.");
  }
  else
  {
    if (settings.IsInt())
      return settings.GetInt();
    else
      throw ConfigException("Configuration Manager: Parse error.");
  }
}

template<>
std::uint64_t ConfigManager::get_value<std::uint64_t>(const Settings settings, const char* key) const
{
  if (key != nullptr)
  {
    if (settings.HasMember(key) && settings[key].IsUint64())
    {
      return settings[key].GetUint64();
    }
    else
      throw ConfigException("Configuration Manager: Parse error.");
  }
  else
  {
    if (settings.IsUint64())
      return settings.GetUint64();
    else
      throw ConfigException("Configuration Manager: Parse error.");
  }
}

template<>
std::int64_t ConfigManager::get_value<std::int64_t>(const Settings settings, const char* key) const
{
  if (key != nullptr)
  {
    if (settings.HasMember(key) && settings[key].IsInt64())
    {
      return settings[key].GetInt64();
    }
    else
      throw ConfigException("Configuration Manager: Parse error.");
  }
  else
  {
    if (settings.IsInt64())
      return settings.GetInt64();
    else
      throw ConfigException("Configuration Manager: Parse error.");
  }
}

template<typename RetType>
std::vector<RetType> ConfigManager::get_array(const Object object) const
{
  if (object.IsArray())
  {
    std::vector<RetType> ret;
    for (rapidjson::SizeType i = 0; i < object.Size(); i++)
    {
      ret.push_back(get_value<RetType>(object[i]));
    }
    return ret;
  }
  else
  {
    throw ConfigException("Configuration Manager: Parse error.");;
  }
}
