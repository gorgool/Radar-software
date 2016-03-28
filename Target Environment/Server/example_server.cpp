#include <chrono>
#include <thread>
#include <iostream>
#include "Server.h"

int main()
{
  TargetEnvironment::Server serv;

  if (serv.load_config(R"(etc/settings.cfg)") != ErrorCode::OK)
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
