#include "ConfigManager.h"
#include <iostream>

void test_6(ConfigManager& mng)
{
  const std::string test_name = "get_object test";

  // 1
  bool flag = true;
  try
  {
    auto& section = mng.get_section("category_2");
    auto& obj = mng.get_object(section, "sub_category");
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
    auto& section = mng.get_section("category_2");
    auto& obj = mng.get_object(section, "sub_category");
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
    auto& section = mng.get_section("category_2");
    auto& obj = mng.get_object(section, "sub_category");
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
    auto& obj = mng.get_object(section, "sdfsdfsdf");
  }
  catch (...)
  {
    flag = false;
  }
  if (flag)
  {
    std::cout << test_name << " - FAIL. Subtest 4.\n";
  }

  std::cout << test_name << " - SUCCESS.\n";
  return;
}