#include "ConfigManager.h"
#include <iostream>

void test_2(ConfigManager& mng)
{
  const std::string test_name = "load_config test";

  // 1
  bool flag = true;
  try
  {
    mng.load_config("config01");
  }
  catch (...)
  {
    flag = false;
  }
  if (flag)
  {
    std::cout << test_name << " - FAIL. Subtest 1.\n";
  }

  mng.set_path(R"(etc/)");
  // 2
  try
  {
    mng.load_config("config01");
  }
  catch (...)
  {
    std::cout << test_name << " - FAIL. Subtest 2.\n";
    return;
  }

  // 3
  flag = true;
  try
  {
    mng.load_config("config000001");
  }
  catch (...)
  {
    flag = false;
  }
  if (flag)
  {
    std::cout << test_name << " - FAIL. Subtest 3.\n";
  }

  // 4
  try
  {
    mng.load_config(R"(config02)");
  }
  catch (...)
  {
    std::cout << test_name << " - FAIL. Subtest 4.\n";
    return;
  }

  // 5
  flag = true;
  try
  {
    mng.load_config(R"(config01.cfg)");
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