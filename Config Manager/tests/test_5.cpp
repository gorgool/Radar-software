#include "ConfigManager.h"
#include <iostream>

void test_5(ConfigManager& mng)
{
  const std::string test_name = "get_section test";

  // 1
  bool flag = true;
  try
  {
    auto& section = mng.get_section("te_settings");
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
    auto& section = mng.get_section("te_settingsss");
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
  try
  {
    auto& section = mng.get_section("category_2");
  }
  catch (...)
  {
    std::cout << test_name << " - FAIL. Subtest 5.\n";
    return;
  }

  // 6
  flag = true;
  try
  {
    auto& section = mng.get_section("");
  }
  catch (...)
  {
    flag = false;
  }
  if (flag)
  {
    std::cout << test_name << " - FAIL. Subtest 6.\n";
  }

  std::cout << test_name << " - SUCCESS.\n";
  return;
}