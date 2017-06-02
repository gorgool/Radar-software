#include <chrono>
#include <thread>
#include <iostream>
#include "Server.h"

#include "Logger/Logger.h"

int main()
{
  TargetEnvironment::Server serv;

  ConfigManager mng;
  mng.set_path(R"(etc/)");
  mng.load_config("settings");

  Logger::load_config(mng);
  Logger::start_session();

  if (serv.load_config(mng) != ErrorCode::OK)
  {
    std::cerr << "load_config error\n";
    return -1;
  };

  if (serv.start() != ErrorCode::OK)
  {
    std::cerr << "start error\n";
    return -1;
  }

  std::cin.ignore();

  if (serv.stop() != ErrorCode::OK)
  {
    std::cerr << "stop error\n";
    return -1;
  }

  return 0;
}
