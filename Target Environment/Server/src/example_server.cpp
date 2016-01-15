#include <chrono>
#include <thread>
#include <iostream>
#include "../include/TargetEnvironmentServer.h"

int main()
{
  boost::asio::io_service service;

  TargetEnvironment::TargetEnvironmentServer serv(service);

  serv.start();

  std::cin.ignore();

  serv.stop();

  return 0;
}
