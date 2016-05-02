#include "ConfigManager.h"
#include <iostream>

void test_9(ConfigManager& mng)
{
  const std::string test_name = "set_value test";

  // 1
  bool flag = true;
  try
  {
    auto& section = mng.get_section("te_settings");

    mng.set_value(section, "ip_address", "192.168.0.100");
    mng.set_value(section, "port", 1921);
    mng.set_value(section, "value1", -1111);

    mng.save_config("config05");
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

    mng.set_value(section, "ip_address", "192.168.0.100");
    mng.set_value(section, "port", 1921);
    mng.set_value(section, "value1", -1111);

    mng.save_config("config05");
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

    mng.set_value(section, "ip_address", "192.168.0.100");
    mng.set_value(section, "port", 1921);
    mng.set_value(section, "value1", -1111);

    mng.save_config("config05");
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
    auto& section = mng.get_section("te_settings");

    mng.set_value(section, "portwer", 1921);
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
    auto& section = mng.get_section("te_settings");

    mng.set_value(section, "ip_address", -1111);
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