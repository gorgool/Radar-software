#include <iostream>
#include "../include/TargetEnvironmentClient.h"
#include <clocale>

int main(int argc, char* argv[])
{
  setlocale(LC_ALL, "");
  boost::asio::ip::tcp::endpoint server_address(boost::asio::ip::address::from_string("127.0.0.1"), 6565);

  TargetEnvironment::TargetEnvironmentClient client;

  auto err = client.connect(server_address);
  if (err != TargetEnvironment::ErrorCode::OK_EC)
  {
    std::cerr << "Error connecting to server.\n";
    std::cin.ignore();
    return -1;
  }

  TargetEnvironment::TargetTable::TableType t;

  err = client.get_targets(t, TargetEnvironment::ClockType::now());

  if (err != TargetEnvironment::ErrorCode::OK_EC)
  {
    std::cerr << "Error getting target list.\n";
    std::cin.ignore();
    return -1;
  }

  for (auto& item : t)
  {
    std::cout << "Target: " << item.first << "\n";
	std::cout << "Coordinates: x = " << item.second.x << " ; y = " << item.second.y << " ; z = " << item.second.z << "\n";
	std::cout << "Velocities: x = " << item.second.vx << " ; y = " << item.second.vy << " ; z = " << item.second.vz << "\n";
	std::cout << "--------------------------------------------------------------------\n";
  }

  std::cin.ignore();

  return 0;
}
