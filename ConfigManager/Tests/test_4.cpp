#include "ConfigManager.h"
#include <iostream>

void test_4(ConfigManager& mng)
{
  const std::string test_name = "update_config test";

  // 1
  bool flag = true;
  try
  {
    std::string update_str = R"({ "value" : 42 })";
    mng.update_config(update_str);
    mng.save_config("config04");
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
    std::string update_str = R"({ "value" : 42 })";
    mng.update_config(update_str);
    mng.save_config("config04");
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
    mng.load_config("config02");
  }
  catch (...)
  {
    std::cout << test_name << " setup FAILed.\n";
  }

  // 3
  try
  {
    std::string update_str = R"({ "value" : 42 })";
    mng.update_config(update_str);
    mng.save_config("config04");
  }
  catch (...)
  {
    std::cout << test_name << " - FAIL. Subtest 3.\n";
    return;
  }

  std::cout << test_name << " - SUCCESS.\n";
  return;
}