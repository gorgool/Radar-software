#include "ConfigManager.h"
#include "Tests.h"
#include <iostream>


int main()
{
  // TEST
  {
    ConfigManager cfmng;

    // CASE 1: set path
    test_1(cfmng);
  }

  {
    ConfigManager cfmng;
    // CASE 2: load config
    test_2(cfmng);
  }

  {
    ConfigManager cfmng;
    // CASE 3: save config
    test_3(cfmng);
  }

  {
    ConfigManager cfmng;
    // CASE 4: update config
    test_4(cfmng);
  }

  {
    ConfigManager cfmng;
    // CASE 5: get section
    test_5(cfmng);
  }

  {
    ConfigManager cfmng;
    // CASE 6: get object
    test_6(cfmng);
  }

  {
    ConfigManager cfmng;
    // CASE 7: get value
    test_7(cfmng);
  }

  {
    ConfigManager cfmng;
    // CASE 8: get array
    test_8(cfmng);
  }

  {
    ConfigManager cfmng;
    // CASE 9: set value
    test_9(cfmng);
  }

  {
    ConfigManager cfmng;
    // CASE 10: set array
    test_10(cfmng);
  }

  std::cin.ignore();

  return 0;
}