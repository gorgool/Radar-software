#include <iostream>
#include <chrono>

using namespace std;

void log_test();

int main()
{
  auto start_time = chrono::high_resolution_clock::now();
  log_test();
  cout << chrono::duration_cast<chrono::duration< double >>(chrono::high_resolution_clock::now() - start_time).count();

  cin.ignore();

  return 0;
}