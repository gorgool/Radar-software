#include "ConfigManager.h"
#include <iostream>
#include <array>

void test_8(ConfigManager& mng)
{
  const std::string test_name = "get_array test";

  // 1
  bool flag = true;
  try
  {
    auto& section = mng.get_section("category_2");

    auto& obj = mng.get_object(section, "sub_category");

    auto& arr_obj = mng.get_object(obj, "arr");

    auto arr = mng.get_array<std::uint32_t>(arr_obj);

    if (arr[0] != 56 || arr[1] != 567 || arr[2] != 67)
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
    auto& section = mng.get_section("category_2");

    auto& obj = mng.get_object(section, "sub_category");

    auto& arr_obj = mng.get_object(obj, "arr");

    auto arr = mng.get_array<std::uint32_t>(arr_obj);

    if (arr[0] != 56 || arr[1] != 567 || arr[2] != 67)
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
    auto& section = mng.get_section("category_2");

    auto& obj = mng.get_object(section, "sub_category");

    auto& arr_obj = mng.get_object(obj, "arr");

    auto arr = mng.get_array<std::uint32_t>(arr_obj);

    if (arr[0] != 56 || arr[1] != 567 || arr[2] != 67)
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
    auto arr = mng.get_array<std::uint32_t>(section);
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