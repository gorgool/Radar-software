#include "ConfigManager.hpp"
#include "Logger.h"

void log_test()
{
  try
  {
    ConfigManager mng;

    mng.set_path(R"(etc/)");
    mng.load_config("config01");

    Logger::log_display(SeverityLevel::Notice, "Test module", "Should be ignored.");

    bool fail_flag = true;
    try
    {
      Logger::start_session();
    }
    catch (...)
    {
      fail_flag = false;
    }

    if (fail_flag)
    {
      std::cerr << "Test failed.\n";
      return;
    }

    Logger::load_config(mng);

    std::uint32_t id = 1001;
    bool done = false;
    while (done != true)
    {
      try
      {
        Logger::start_session(id);
      }
      catch (...)
      {
        id++;
        continue;
      }
      done = true;
    }

    if (Logger::get_session_id() != id)
    {
      std::cerr << "Test failed.\n";
      return;
    }

    Logger::log_file(SeverityLevel::Notice, "Test module", "Test 1.");
    Logger::log_display(SeverityLevel::Notice, "Test module", "Test 1.");
    Logger::log_dispfile(SeverityLevel::Error, "Test module", "Test 2.");
    Logger::log_file(SeverityLevel::Debug, "Test module", "Test 3.");
    Logger::log_display(SeverityLevel::Debug, "Test module", "Test 3.");

    Logger::close_session();

    Logger::log_display(SeverityLevel::Error, "Test module", "Should be ignored.");

    Logger::start_session();
    if (Logger::get_session_id() == 1001)
    {
      std::cerr << "Test failed.\n";
      return;
    }

    Logger::log_file(SeverityLevel::Notice, "Test module 2", "Test 1.");
    Logger::log_display(SeverityLevel::Notice, "Test module 2", "Test 1.");
    Logger::log_dispfile(SeverityLevel::Error, "Test module 2", "Test 2.");
    Logger::log_file(SeverityLevel::Debug, "Test module 2", "Test 3.");
    Logger::log_display(SeverityLevel::Debug, "Test module 2", "Test 3.");
  }
  catch (...)
  {
    std::cerr << "Test failed.\n";
    return;
  }

  std::cerr << "Test pass.\n";
}