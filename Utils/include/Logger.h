#pragma once

#include <string>
#include <fstream>
#include <array>
#include <vector>
#include <future>
#include <chrono>
#include <stdexcept>
#include <atomic>
#include "FormatString.h"

#undef DEBUG_LOG
//#define DEBUG_LOG

#ifndef DEBUG_LOG
  #define DLOG(text) ;
  #define DFLOG(text) ;
#endif
#ifdef DEBUG_LOG
  #define DLOG(text) Utils::Log.log_display(text);
  #define DFLOG(text) Utils::Log.log(text);
#endif

namespace Utils
{
  class Logger
  {
  public:

    Logger() : current_buffer(buffer_size, std::string("Init buffer value, for memory preallocation")), second_buffer(buffer_size, std::string("Init buffer value, for memory preallocation")), buffer_ptr(0)
    {
      out_file.open(std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) + ".log");
      if (out_file.bad())
      {
      	puts("Error creating logger.\n");
        throw std::logic_error("Can't create output log file. \n");
      }
    }

    // Add log entry to output file stream
    template < typename T >
    void log(T&& entry)
    {
      std::lock_guard<std::mutex> lg(m);
      current_buffer[buffer_ptr] = std::forward<T>(entry);
      buffer_ptr++;
      if (buffer_ptr.load() == buffer_size)
      {
        std::swap(current_buffer, second_buffer);
        buffer_ptr = 0;
        flush(0, buffer_size);
      }
    }

    // Display log entry in standart output
    void log_display(const std::string& entry) const
    {
      puts(entry.c_str());
    }

    // Display log entry in standart output
    void log_display(const char* entry) const
    {
      puts(entry);
    }

    // Save all entries if file stream and create new file.
    void reset()
    {
      std::lock_guard<std::mutex> lg(m);
      if (out_file.is_open())
      {
        std::swap(current_buffer, second_buffer);
        auto future = flush(0, buffer_ptr);
        future.wait();
        out_file.close();
      }

      out_file.open(std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) + ".log");

      if (out_file.bad())
        throw std::logic_error("Can't create output log file. \n");

      buffer_ptr = 0;
    }

    ~Logger()
    {
      std::swap(current_buffer, second_buffer);
      auto future = flush(0, buffer_ptr);
      future.wait();
    }

  private:

    std::mutex m;
    const static std::size_t buffer_size = 50;
    //using buffer_type = std::array<std::string, buffer_size>;
    using buffer_type = std::vector<std::string>;

    std::future<void> flush(const std::size_t from, const std::size_t to)
    {
      return std::async(std::launch::async, [this](const std::size_t from, const std::size_t to)
      {
        for (auto idx = from; idx < to; ++idx)
          out_file << second_buffer[idx];
      }, from, to);
    }

    buffer_type current_buffer;
    buffer_type second_buffer;

    std::atomic<std::size_t> buffer_ptr;

    std::ofstream out_file;
  };

  extern Logger Log;
};
