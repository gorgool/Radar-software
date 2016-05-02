#pragma once
#include <string>

struct Message
{
  std::string _type;

  std::string _text;
  
public:
  Message(const std::string& type, const std::string& msg) : _type(type), _text(msg) {};

  std::string serialize() const
  {
    return _type + " " + _text;
  }
};
