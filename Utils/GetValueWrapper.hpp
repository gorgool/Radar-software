#pragma once
#include <rapidjson/document.h>
#include <rapidjson/writer.h>

template<typename T>
T get_value(const rapidjson::Value& root, const char* key = nullptr)
{
  // Not implemented for generic type. Specializations see below.
  return T();
}

template<>
double get_value(const rapidjson::Value& root, const char* key)
{
  if (key != nullptr)
  {
    if (root[key].IsNumber() && root.HasMember(key))
      return root[key].GetDouble();
    else
      throw std::system_error(std::error_code(), "Parse error.");
  }
  else
  {
    if (root.IsNumber())
      return root.GetDouble();
    else
      throw std::system_error(std::error_code(), "Parse error.");
  }
}

template<>
std::string get_value(const rapidjson::Value& root, const char* key)
{
  if (key != nullptr)
  {
    if (root.HasMember(key) && root[key].IsString())
    {
      return root[key].GetString(); 
    }
    else
      throw std::system_error(std::error_code(), "Parse error.");
  }
  else
  {
    if (root.IsString())
      return root.GetString();
    else
      throw std::system_error(std::error_code(), "Parse error.");
  }
}

template<>
std::uint32_t get_value(const rapidjson::Value& root, const char* key)
{
  if (key != nullptr)
  {
    if (root.HasMember(key) && root[key].IsUint())
    {
      return root[key].GetUint();
    }
    else
      throw std::system_error(std::error_code(), "Parse error.");
  }
  else
  {
    if (root.IsUint())
    {
      return root.GetUint();
    }
    else
      throw std::system_error(std::error_code(), "Parse error.");
  }
    
}

template<>
std::uint64_t get_value(const rapidjson::Value& root, const char* key)
{
  if (key != nullptr)
  {
    if (root.HasMember(key) && root[key].IsUint64())
    {
      return root[key].GetUint64();
    }
    else
      throw std::system_error(std::error_code(), "Parse error.");
  }
  else
  {
    if (root.IsUint64())
      return root.GetUint64();
    else
      throw std::system_error(std::error_code(), "Parse error.");

  }
}

template<>
std::int32_t get_value(const rapidjson::Value& root, const char* key)
{
  if (key != nullptr)
  {
    if (root.HasMember(key) && root[key].IsInt())
      return root[key].GetInt();
    else
      throw std::system_error(std::error_code(), "Parse error.");
  }
  else
  {
    if (root.IsInt())
      return root.GetInt();
    else
      throw std::system_error(std::error_code(), "Parse error.");

  }
}

template<>
std::int64_t get_value(const rapidjson::Value& root, const char* key)
{
  if (key != nullptr)
  {
    if (root.HasMember(key) && root[key].IsInt64())
    {
      return root[key].GetInt64();
    }
    else
      throw std::system_error(std::error_code(), "Parse error.");
  }
  else
  {
    if (root.IsInt64())
      return root.GetInt64();
    else
      throw std::system_error(std::error_code(), "Parse error.");
  }
}

rapidjson::Document get_nested_object(rapidjson::Document& root, const char* key)
{
  if (root.HasMember(key) && root[key].IsObject())
  {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    root[key].Accept(writer);
    rapidjson::StringStream s(buffer.GetString());

    rapidjson::Document nested_object;
    nested_object.ParseStream(s);

    if (nested_object.HasParseError())
    {
      throw std::system_error(std::error_code(), "Parse error.");
    }
    else
      return nested_object;
  }
  else
    throw std::system_error(std::error_code(), "Parse error.");
}

template<typename T, std::size_t Size>
std::array<T, Size> get_array(const rapidjson::Value& arr)
{
  if (arr.IsArray())
  {
    assert(arr.Size() == Size);
    std::array<T, Size> ret;
    for (rapidjson::SizeType i = 0; i < arr.Size(); i++)
    {
      ret[i] = get_value<T>(arr[i]);
    }
    return ret;
  }
  else
  {
    throw std::system_error(std::error_code(), "Parse error.");
  }
}
