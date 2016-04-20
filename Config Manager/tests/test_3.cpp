#include "ConfigManager.h"
#include <iostream>

void test_3(ConfigManager& mng)
{
  const std::string test_name = "save_config test";

  // 1
  bool flag = true;
  try
  {
    mng.save_config("config03");
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
    mng.save_config("config03");
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
    mng.save_config("config03");
  }
  catch (...)
  {
    std::cout << test_name << " - FAIL. Subtest 3.\n";
    return;
  }

  std::cout << test_name << " - SUCCESS.\n";
  return;
}