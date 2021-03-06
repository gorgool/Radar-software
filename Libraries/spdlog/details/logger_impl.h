//
// Copyright(c) 2015 Gabi Melman.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#pragma once

#include "../logger.h"

#include <memory>
#include <string>

// create logger with given name, sinks and the default pattern formatter
// all other ctors will call this one
template<class It>
inline spdlog::logger::logger(const std::string& logger_name, const It& begin, const It& end) :
    _name(logger_name),
    _sinks(begin, end),
    _formatter(std::make_shared<pattern_formatter>("%+"))
{

    // no support under vs2013 for member initialization for std::atomic
    _level = level::info;
    _flush_level = level::off;
}

// ctor with sinks as init list
inline spdlog::logger::logger(const std::string& logger_name, sinks_init_list sinks_list) :
    logger(logger_name, sinks_list.begin(), sinks_list.end()) {}


// ctor with single sink
inline spdlog::logger::logger(const std::string& logger_name, spdlog::sink_ptr single_sink) :
    logger(logger_name,
{
    single_sink
}) {}


inline spdlog::logger::~logger() = default;


inline void spdlog::logger::set_formatter(spdlog::formatter_ptr msg_formatter)
{
    _set_formatter(msg_formatter);
}

inline void spdlog::logger::set_pattern(const std::string& pattern)
{
    _set_pattern(pattern);
}

//
// log only if given level>=logger's log level
//


template <typename... Args>
inline spdlog::details::line_logger spdlog::logger::_log_if_enabled(level::level_enum lvl, const char* fmt, const Args&... args)
{
    bool msg_enabled = should_log(lvl);
    details::line_logger l(this, lvl, msg_enabled);
    l.write(fmt, args...);
    return l;
}

inline spdlog::details::line_logger spdlog::logger::_log_if_enabled(level::level_enum lvl)
{
    return details::line_logger(this, lvl, should_log(lvl));
}

template<typename T>
inline spdlog::details::line_logger spdlog::logger::_log_if_enabled(level::level_enum lvl, const T& msg)
{
    bool msg_enabled = should_log(lvl);
    details::line_logger l(this, lvl, msg_enabled);
    l << msg;
    return l;
}

//
// logger.info(cppformat_string, arg1, arg2, arg3, ...) call style
//
template <typename... Args>
inline spdlog::details::line_logger spdlog::logger::trace(const char* fmt, const Args&... args)
{
    return _log_if_enabled(level::trace, fmt, args...);
}

template <typename... Args>
inline spdlog::details::line_logger spdlog::logger::debug(const char* fmt, const Args&... args)
{
    return _log_if_enabled(level::debug, fmt, args...);
}

template <typename... Args>
inline spdlog::details::line_logger spdlog::logger::info(const char* fmt, const Args&... args)
{
    return _log_if_enabled(level::info, fmt, args...);
}

template <typename... Args>
inline spdlog::details::line_logger spdlog::logger::notice(const char* fmt, const Args&... args)
{
    return _log_if_enabled(level::notice, fmt, args...);
}

template <typename... Args>
inline spdlog::details::line_logger spdlog::logger::warn(const char* fmt, const Args&... args)
{
    return _log_if_enabled(level::warn, fmt, args...);
}

template <typename... Args>
inline spdlog::details::line_logger spdlog::logger::error(const char* fmt, const Args&... args)
{
    return _log_if_enabled(level::err, fmt, args...);
}

template <typename... Args>
inline spdlog::details::line_logger spdlog::logger::critical(const char* fmt, const Args&... args)
{
    return _log_if_enabled(level::critical, fmt, args...);
}

template <typename... Args>
inline spdlog::details::line_logger spdlog::logger::alert(const char* fmt, const Args&... args)
{
    return _log_if_enabled(level::alert, fmt, args...);
}

template <typename... Args>
inline spdlog::details::line_logger spdlog::logger::emerg(const char* fmt, const Args&... args)
{
    return _log_if_enabled(level::emerg, fmt, args...);
}

//
// logger.info(msg) << ".." call style
//
template<typename T>
inline spdlog::details::line_logger spdlog::logger::trace(const T& msg)
{
    return _log_if_enabled(level::trace, msg);
}

template<typename T>
inline spdlog::details::line_logger spdlog::logger::debug(const T& msg)
{
    return _log_if_enabled(level::debug, msg);
}


template<typename T>
inline spdlog::details::line_logger spdlog::logger::info(const T& msg)
{
    return _log_if_enabled(level::info, msg);
}

template<typename T>
inline spdlog::details::line_logger spdlog::logger::notice(const T& msg)
{
    return _log_if_enabled(level::notice, msg);
}

template<typename T>
inline spdlog::details::line_logger spdlog::logger::warn(const T& msg)
{
    return _log_if_enabled(level::warn, msg);
}

template<typename T>
inline spdlog::details::line_logger spdlog::logger::error(const T& msg)
{
    return _log_if_enabled(level::err, msg);
}

template<typename T>
inline spdlog::details::line_logger spdlog::logger::critical(const T& msg)
{
    return _log_if_enabled(level::critical, msg);
}

template<typename T>
inline spdlog::details::line_logger spdlog::logger::alert(const T& msg)
{
    return _log_if_enabled(level::alert, msg);
}

template<typename T>
inline spdlog::details::line_logger spdlog::logger::emerg(const T& msg)
{
    return _log_if_enabled(level::emerg, msg);
}




//
// logger.info() << ".." call  style
//
inline spdlog::details::line_logger spdlog::logger::trace()
{
    return _log_if_enabled(level::trace);
}

inline spdlog::details::line_logger spdlog::logger::debug()
{
    return _log_if_enabled(level::debug);
}

inline spdlog::details::line_logger spdlog::logger::info()
{
    return _log_if_enabled(level::info);
}

inline spdlog::details::line_logger spdlog::logger::notice()
{
    return _log_if_enabled(level::notice);
}

inline spdlog::details::line_logger spdlog::logger::warn()
{
    return _log_if_enabled(level::warn);
}

inline spdlog::details::line_logger spdlog::logger::error()
{
    return _log_if_enabled(level::err);
}

inline spdlog::details::line_logger spdlog::logger::critical()
{
    return _log_if_enabled(level::critical);
}

inline spdlog::details::line_logger spdlog::logger::alert()
{
    return _log_if_enabled(level::alert);
}

inline spdlog::details::line_logger spdlog::logger::emerg()
{
    return _log_if_enabled(level::emerg);
}


// always log, no matter what is the actual logger's log level
template <typename... Args>
inline spdlog::details::line_logger spdlog::logger::force_log(level::level_enum lvl, const char* fmt, const Args&... args)
{
    details::line_logger l(this, lvl, true);
    l.write(fmt, args...);
    return l;
}

//
// name and level
//
inline const std::string& spdlog::logger::name() const
{
    return _name;
}

inline void spdlog::logger::set_level(spdlog::level::level_enum log_level)
{
    _level.store(log_level);
}

inline void spdlog::logger::flush_on(level::level_enum log_level)
{
    _flush_level.store(log_level);
}

inline spdlog::level::level_enum spdlog::logger::level() const
{
    return static_cast<spdlog::level::level_enum>(_level.load(std::memory_order_relaxed));
}

inline bool spdlog::logger::should_log(spdlog::level::level_enum msg_level) const
{
    return msg_level >= _level.load(std::memory_order_relaxed);
}

//
// protected virtual called at end of each user log call (if enabled) by the line_logger
//
inline void spdlog::logger::_log_msg(details::log_msg& msg)
{
    _formatter->format(msg);
    for (auto &sink : _sinks)
        sink->log(msg);

    const auto flush_level = _flush_level.load(std::memory_order_relaxed);
    if (msg.level >= flush_level)
        flush();
}

inline void spdlog::logger::_set_pattern(const std::string& pattern)
{
    _formatter = std::make_shared<pattern_formatter>(pattern);
}
inline void spdlog::logger::_set_formatter(formatter_ptr msg_formatter)
{
    _formatter = msg_formatter;
}

inline void spdlog::logger::flush()
{
    for (auto& sink : _sinks)
        sink->flush();
}
