#pragma once

#include <string>
#include "Utils/ErrorExceptions.h"
#include <rapidjson/document.h>
#include <boost/filesystem.hpp>

class ConfigManager
{

  /** @brief Loaded JSON configuration. */
  rapidjson::Document _json_config;

  /** @brief Name of the configuration. */
  std::string _config_name;

  /** @brief Pathname of the directory with configuration files. */
  boost::filesystem::path _path;

  /** @brief true if configuration loaded. */
  bool _config_loaded = false;


public:

  /** @brief Section's settings type. */
  using Settings = const rapidjson::Value&;

  /** @brief JSON object type. */
  using Object = const rapidjson::Value&;

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

  void set_path(const std::string& path);

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

  void load_config(const std::string& config_name);

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

  void save_config(const std::string& config_name) const;

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

  void update_config(const std::string& config_str);

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

  std::string config_name() const noexcept;

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

  std::string config_path() const noexcept;

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

  std::string get_config_str() const noexcept;

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

  const Settings get_section(const std::string& section_name) const;

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

  Object get_object(const Settings settings, const char* key) const;

  /**
   * @fn  template<typename RetType> RetType ConfigManager::get_value(const Settings& settings, const char* key) const;
   *
   * @brief Gets a value.
   *
   * @author  Gorgool
   * @date  29.03.2016
   *
   * @exception ConfigException Thrown if the key don't exists or can't cast to RetType.
   *
   * @tparam  RetType Type of the return value.
   * @param settings  Section settings or Object.
   * @param key       The key.
   *
   * @return  The value of type RetType.
   */

  template<typename RetType> 
  RetType get_value(const Settings settings, const char* key = nullptr) const;

  /**
   * @fn  template<typename RetType, std::size_t Size> std::array<RetType, Size> ConfigManager::get_array(const Object& settings) const;
   *
   * @brief Convert object to an array.
   *
   * @author  Gorgool
   * @date  29.03.2016
   *
   * @exception ConfigException Thrown if the object can't be cast to an array.
   *                            
   * @tparam  RetType  Type of the array.
   * @tparam   Size    Size of the array.
   * @param   object   JSON Object.
   *
   * @return  A std::array&lt;RetType,Size&gt;
   */

  template<typename RetType> 
  std::vector<RetType> get_array(const Object object) const;

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

  void set_value(Settings settings, const char* key, const std::string& value);

  void set_value(Settings settings, const char* key, const double value);

  void set_value(Settings settings, const char* key, const std::uint32_t value);

  void set_value(Settings settings, const char* key, const std::int32_t value);

  void set_value(Settings settings, const char* key, const std::uint64_t value);

  void set_value(Settings settings, const char* key, const std::int64_t value);


  /**
   * @fn  template<typename ArrayType> void ConfigManager::set_array(Object& object, ArrayType arr);
   *
   * @brief Sets an array.
   *
   * @author  Gorgool
   * @date  29.03.2016
   *
   * @exception ConfigException Thrown if object is not an JSON array.
   *
   * @tparam  ArrayType Type of the array.
   * @param [in,out]  object  The JSON object.
   * @param arr               The array.
   */

  template<typename ArrayType> 
  void set_array(Object object, const std::vector<ArrayType>& arr);
};

/* Specializations */
  template<>
  std::string ConfigManager::get_value<std::string>(const Settings settings, const char* key) const;

  template<>
  double ConfigManager::get_value<double>(const Settings settings, const char* key) const;

  template<>
  std::uint32_t ConfigManager::get_value<std::uint32_t>(const Settings settings, const char* key) const;

  template<>
  std::int32_t ConfigManager::get_value<std::int32_t>(const Settings settings, const char* key) const;

  template<>
  std::uint64_t ConfigManager::get_value<std::uint64_t>(const Settings settings, const char* key) const;

  template<>
  std::int64_t ConfigManager::get_value<std::int64_t>(const Settings settings, const char* key) const;


template<typename ArrayType>
void ConfigManager::set_array(Object object, const std::vector<ArrayType>& arr)
{
  if (object.IsArray())
  {
    auto& obj = const_cast<rapidjson::Value&>(object);
    obj.Clear();
    for (auto& item : arr)
    {
      obj.PushBack(item, _json_config.GetAllocator());
    }
  }
  else
  {
    throw ConfigException("Configuration Manager: Parse error.");;
  }
}
