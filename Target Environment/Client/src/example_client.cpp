#include <iostream>
#include "../include/TargetEnvironmentClient.h"
#include "../../../Utils/include/ExecTime.hpp"
#include <clocale>
#include <random>
#include <atomic>
#include <thread>
#include <vector>
#include <string>

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

  if (argc != 3)
  {
    std::cerr << "Wrong number of arguments. Usage: te_client server_ip number_of_threads.\n";
    return -1;
  }

  std::string serv_addr = argv[1];

  boost::system::error_code ec;
  auto server_ip = boost::asio::ip::address::from_string(serv_addr, ec);

  if (ec)
  {
    std::cerr << "Wrong server ip format. Example: te_client 127.0.0.1 4.\n";
    return -1;
  }

  size_t n_threads = 1;
  try
  {
    n_threads = std::stoi(argv[2]);
  }
  catch (...)
  {
    std::cerr << "Wrong number of threads. Should be positive interger. Using default value (1). Example: te_client 127.0.0.1 4.\n";
  }

  boost::asio::ip::tcp::endpoint server_address(server_ip, 6565);

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<std::uint32_t> id(0, std::numeric_limits<std::uint32_t>::max());

  std::vector<std::size_t> counters(n_threads, 0);
  bool stop_flag = true;

  auto worker = [&](std::size_t idx)
  {
    setlocale(LC_ALL, "");
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

    while (stop_flag)
    {
      TargetEnvironment::TargetTable::TableType t;
      err = client.get_targets(t, TargetEnvironment::ClockType::now());
      if (err != TargetEnvironment::ErrorCode::OK_EC)
      {
        std::cerr << "Error getting target list.\n";
        std::cin.ignore();
        return -1;
      }
      counters[idx]++;
    }
  };

  std::vector<std::thread> thread_pool;
  for (size_t i = 0; i < n_threads; ++i)
  {
    thread_pool.push_back(std::thread(worker, i));
  }

  std::vector<std::thread> monitor_thread_pool;
  for (size_t i = 0; i < n_threads; ++i)
  {
    monitor_thread_pool.push_back(std::thread([&](std::size_t& c) 
    { 
      while (stop_flag)
      {
        printf("%zu\n", c);
        std::this_thread::sleep_for(std::chrono::seconds(1));
      }
    }, std::ref(counters[i])));
  }

  std::this_thread::sleep_for(std::chrono::seconds(20));
  
  stop_flag = false;
  
  for (auto& th : thread_pool)
    th.join();

  for (auto& th : monitor_thread_pool)
    th.join();

  std::cin.ignore();

  return 0;
}
