#include "ConfigManager.h"
#include <iostream>

void test_1(ConfigManager& mng)
{
  const std::string test_name = "set_path test";

  // 1
  try
  {
    mng.set_path(R"(etc/)");
  }
  catch (...)
  {
    std::cout << test_name << " - FAIL. Subtest 1.\n";
    return;
  }

  // 2
  bool flag = true;
  try
  {
    mng.set_path("");
  }
  catch (...)
  {
    flag = false;
  }
  if (flag)
  {
    std::cout << test_name << " - FAIL. Subtest 2.\n";
  }

  // 3 ------------------!! WINDOWS ONLY !!---------------------
  try
  {
    mng.set_path(R"(D:\���������\Sources\Utilities\Configuration Manager\etc)");
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
    mng.set_path("ghsd");
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
