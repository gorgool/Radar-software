#include <chrono>
#include <thread>
#include <iostream>
#include "../include/TargetEnvironmentServer.h"

int main(int argc, char* argv[])
{
  boost::asio::io_service service;

  TargetEnvironment::TargetEnvironmentServer serv(service);

  serv.start();

  std::cin.ignore();

  serv.stop();

  return 0;
}
