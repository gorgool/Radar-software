#pragma once
/*
  Convert format string (like in printf()) into std::string

  NOTES:
  On Windows - _snprintf_s
  On Linux - snprintf
*/

#include <memory>
#include <iostream>
#include <string>
#include <cstdio>
#include <array>
#include <vector>

#ifdef __linux__
template<typename ... Args>
int snprintf_impl(char *buf, size_t buf_size, 
									const char* format_str, Args ... args)
{
	return snprintf(buf, buf_size, format_str, args ...);
}
#endif

#ifdef _WIN32
template<typename ... Args>
int snprintf_impl(char *buf, size_t buf_size,
									const char* format_str, Args ... args)
{
	return _snprintf_s(buf, buf_size, buf_size, format_str, args ...);
}
#endif

namespace Utils
{
  template<typename ... Args>
  std::string string_format(const std::string& format, Args ... args)
  {
    std::array<char, 1024> stack_buf;
    char *buf = stack_buf.data();

    size_t size = 1024;
    std::vector<char> dynamic_buf;

    while (true)
    {
      int needed = snprintf_impl(buf, size, format.c_str(), args ...);
      if (needed <= static_cast<int>(size) && needed >= 0)
        // It's OK
        return std::string(buf, static_cast<size_t>(needed));

      // Resizing
      size = (needed > 0) ? (needed + 1) : (size * 2);
      dynamic_buf.resize(size);
      buf = dynamic_buf.data();
    }
  }
};
