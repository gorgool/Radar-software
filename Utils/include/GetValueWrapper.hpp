#pragma once
#include <rapidjson\document.h>

template<typename T>
T get_value(rapidjson::Document& root, const char* key)
{
  // Not implemented for generic type. Specializations see below.
  static_assert(false);
  return T();
}

template<>
std::string get_value(rapidjson::Document& root, const char* key)
{
  if (root.HasMember(key) && root[key].IsString())
  {
    return root[key].GetString();
  }
  else
    throw std::system_error(std::error_code(), "Parse error.");
}

template<>
std::uint32_t get_value(rapidjson::Document& root, const char* key)
{
  if (root.HasMember(key) && root[key].IsUint())
  {
    return root[key].GetUint();
  }
  else
    throw std::system_error(std::error_code(), "Parse error.");
}

template<>
std::uint64_t get_value(rapidjson::Document& root, const char* key)
{
  if (root.HasMember(key) && root[key].IsUint64())
  {
    return root[key].GetUint64();
  }
  else
    throw std::system_error(std::error_code(), "Parse error.");
}

template<>
double get_value(rapidjson::Document& root, const char* key)
{
  if (root.HasMember(key) && root[key].IsNumber())
  {
    return root[key].GetDouble();
  }
  else
    throw std::system_error(std::error_code(), "Parse error.");
}