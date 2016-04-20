#include "ConfigManager.h"
#include <iostream>
#include <array>

void test_10(ConfigManager& mng)
{
  const std::string test_name = "set_array test";

  // 1
  bool flag = true;
  try
  {
    auto& section = mng.get_section("category_2");

    auto& obj = mng.get_object(section, "sub_category");

    auto& arr_obj = mng.get_object(obj, "arr");

    std::vector<double> arr{ 12.0, 0.0, 0.004, 15.4, 45, 9876.0 };

    mng.set_array<double>(arr_obj, arr);

    mng.save_config("config06");
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

    auto& arr_obj = mng.get_object(obj, "arr");

    std::vector<double> arr{ 12.0, 0.0, 0.004, 15.4, 45, 9876.0 };

    mng.set_array<double>(arr_obj, arr);

    mng.save_config("config06");
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

    auto& arr_obj = mng.get_object(obj, "arr");

    std::vector<double> arr{ 12.0, 0.0, 0.004, 15.4, 45, 9876.0 };

    mng.set_array<double>(arr_obj, arr);

    mng.save_config("config06");
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
    std::vector<double> arr{ 12.0, 0.0, 0.004, 15.4, 45, 9876.0 };
    mng.set_array<double>(section, arr);
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