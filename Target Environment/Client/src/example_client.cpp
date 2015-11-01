#include <iostream>
#include "../include/TargetEnvironmentClient.h"
#include <clocale>

std::ostream& operator<<(std::ostream& out, std::array<double,6>& arr)
{
  for (auto&& item : arr)
  {
    out << item << " ; ";
  }
  out << std::endl;
  return out;
}


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
	  std::cout << "X vector = " << item.second.x << "\n";
	  std::cout << "Y vector = " << item.second.y << "\n";
    std::cout << "Z vector = " << item.second.z << "\n";
	  std::cout << "--------------------------------------------------------------------\n";
  }

  std::cin.ignore();

  return 0;
}
