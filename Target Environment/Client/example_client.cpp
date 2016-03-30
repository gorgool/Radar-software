#include <iostream>
#include "Client.h"
#include "Utils/ExecTime.hpp"
#include "CoordinateSystems/CSUtils.h"
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

int max_connection_test(const std::uint32_t _max = 100)
{
  int counter = 0;
  std::vector<TargetEnvironment::Client> clients_pool(101);
  auto client_iterator = clients_pool.begin();
  while (true)
  {
    auto err = client_iterator->load_config(R"(etc/settings.cfg)");
    if (err != ErrorCode::OK)
    {
      std::cerr << "Error loading config.\n";
      break;
    }

    err = client_iterator->connect();
    if (err != ErrorCode::OK)
    {
      break;
    }
    counter++;
    client_iterator++;
    if (counter == _max + 1)
    {
      std::cerr << "Error in max_connection_test.\n";
      break;
    }
  }
  return counter;
}


int main(int argc, char* argv[])
{
  setlocale(LC_ALL, "");

  if (argc != 2)
  {
    std::cerr << "Wrong number of arguments. Usage: te_client number_of_threads.\n";
    return -1;
  }

  size_t n_threads = 1;
  try
  {
    n_threads = std::stoi(argv[1]);
  }
  catch (...)
  {
    std::cerr << "Wrong number of threads. Should be positive interger. Using default value (1).\nExample: te_client 4\n";
  }

  std::vector<std::size_t> counters(n_threads, 0);
  std::vector<std::size_t> targets_counters(n_threads, 0);
  bool stop_flag = true;

  auto worker = [&](std::size_t idx)
  {
    setlocale(LC_ALL, "");
    
    TargetEnvironment::SearchArea area(0.0, 0.0, 0.0, 3000000, 45.0, 25.0, 10.0, 40.0);
    CSUtils::GCSPoint radar_location(44.392087, -68.204052, 100);

    TargetEnvironment::Client client;

    auto err = client.load_config(R"(etc/settings.cfg)");
    if (err != ErrorCode::OK)
    {
      std::cerr << "Error loading config.\n";
      std::cin.ignore();
      return;
    }

    err = client.connect();
    if (err != ErrorCode::OK)
    {
      std::cerr << "Error connecting to server.\n";
      std::cin.ignore();
      return;
    }

    while (stop_flag)
    {
      TargetEnvironment::TargetTable t;
      err = client.get_targets(t, area, radar_location, TargetEnvironment::ClockType::now());
      if (err != ErrorCode::OK)
      {
        std::cerr << "Error getting target list.\n";
        std::cin.ignore();
        return;
      }
      targets_counters[idx] += t.size();
      counters[idx]++;
    }
  };

  //auto c = max_connection_test();
  //std::cout << "Connections test :" << ((c == 100) ? " pass" : " fail");


  std::vector<std::thread> thread_pool;
  for (size_t i = 0; i < n_threads; ++i)
  {
    thread_pool.push_back(std::thread(worker, i));
  }

  std::vector<std::thread> monitor_thread_pool;
  for (size_t i = 0; i < n_threads; ++i)
  {
    monitor_thread_pool.push_back(std::thread([&](std::size_t& c, std::size_t& targets)
    { 
      while (stop_flag)
      {
        printf("Requests : %zu \t Targets : %zu \n", c, targets);
        std::this_thread::sleep_for(std::chrono::seconds(1));
      }
    }, std::ref(counters[i]), std::ref(targets_counters[i])));
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
