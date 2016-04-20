#include "ConfigManager.h"
#include <iostream>

void test_7(ConfigManager& mng)
{
  const std::string test_name = "get_value test";

  // 1
  bool flag = true;
  try
  {
    auto& section = mng.get_section("te_settings");

    if (mng.get_value<std::string>(section, "ip_address") != "127.0.0.1")
      throw std::exception();

    if (mng.get_value<std::uint32_t>(section, "port") != 6565)
      throw std::exception();

    if (mng.get_value<std::int32_t>(section, "value1") != -76)
      throw std::exception();

    if ((mng.get_value<double>(section, "value2") - (-89.0965)) > std::numeric_limits<double>::epsilon())
      throw std::exception();

    std::uint64_t val = 100000000000;
    auto fdg = mng.get_value<std::uint64_t>(section, "max_connections");
    if (mng.get_value<std::uint64_t>(section, "max_connections") != val)
      throw std::exception();
  }
  catch (...)
  {
    flag = false;
  }
  if (flag)
  {
    std::cout << test_name << " - FAIL. Subtest 1.\n";
  }

  try
  {
    mng.set_path(R"(etc/)");
  }
  catch (...)
  {
    std::cout << test_name << " setup FAILed.\n";
  }

  // 2
  flag = true;
  try
  {
    auto& section = mng.get_section("te_settings");

    if (mng.get_value<std::string>(section, "ip_address") != "127.0.0.1")
      throw std::exception();

    if (mng.get_value<std::uint32_t>(section, "port") != 6565)
      throw std::exception();

    if (mng.get_value<std::int32_t>(section, "value1") != -76)
      throw std::exception();

    if ((mng.get_value<double>(section, "value2") - (-89.0965)) > std::numeric_limits<double>::epsilon())
      throw std::exception();

    std::uint64_t val = 100000000000;
    auto fdg = mng.get_value<std::uint64_t>(section, "max_connections");
    if (mng.get_value<std::uint64_t>(section, "max_connections") != val)
      throw std::exception();
  }
  catch (...)
  {
    flag = false;
  }
  if (flag)
  {
    std::cout << test_name << " - FAIL. Subtest 2.\n";
  }

  try
  {
    mng.load_config("config01");
  }
  catch (...)
  {
    std::cout << test_name << " setup FAILed.\n";
  }

  // 3
  try
  {
    auto& section = mng.get_section("te_settings");

    if (mng.get_value<std::string>(section, "ip_address") != "127.0.0.1")
      throw std::exception();

    if (mng.get_value<std::uint32_t>(section, "port") != 6565)
      throw std::exception();

    if (mng.get_value<std::int32_t>(section, "value1") != -76)
      throw std::exception();

    if ( (mng.get_value<double>(section, "value2") - (-89.0965)) > std::numeric_limits<double>::epsilon())
      throw std::exception();

    std::uint64_t val = 100000000000;
    auto fdg = mng.get_value<std::uint64_t>(section, "max_connections");
    if (mng.get_value<std::uint64_t>(section, "max_connections") != val)
      throw std::exception();
  }
  catch (...)
  {
    std::cout << test_name << " - FAIL. Subtest 3.\n";
    return;
  }

  // 4
  flag = true;
  try
  {
    auto& section = mng.get_section("category_2");
    mng.get_value<std::string>(section, "var1");
  }
  catch (...)
  {
    flag = false;
  }
  if (flag)
  {
    std::cout << test_name << " - FAIL. Subtest 4.\n";
  }

  // 5
  flag = true;
  try
  {
    auto& section = mng.get_section("category_2");
    mng.get_value<std::int32_t>(section, "var2");
  }
  catch (...)
  {
    flag = false;
  }
  if (flag)
  {
    std::cout << test_name << " - FAIL. Subtest 5.\n";
  }

  std::cout << test_name << " - SUCCESS.\n";
  return;
}