#include <iostream>
#include "../include/TargetEnvironmentClient.h"
#include <clocale>
#include <random>

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

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<std::size_t> id(0, std::numeric_limits<std::size_t>::max());
  auto client_id = id(gen);
  TargetEnvironment::ReferencePointDesc rfp(client_id, 44.392087, -68.204052, 100, 3000000, 45.0, 25.0, 10.0, 40.0);
  
  TargetEnvironment::TargetEnvironmentClient client(rfp);

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
