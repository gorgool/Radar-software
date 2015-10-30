#include <chrono>
#include <utility>

template< 
  typename RetT, 
  typename FuncT, 
  typename ... argsT >
double exec_time( RetT& ret, FuncT&& func, argsT& ... params )
{
  using namespace std::chrono;
  high_resolution_clock::time_point start_time =  high_resolution_clock::now();
  ret = func( forward<argsT>(params) ... );
  return duration_cast< duration< double > >( high_resolution_clock::now() - start_time ).count();
}

template<typename FuncT, typename ... argsT>
double noret_exec_time(FuncT&& func, argsT& ... params)
{
  using namespace std::chrono;
  high_resolution_clock::time_point start_time =  high_resolution_clock::now();
  func( forward<argsT>(params) ...);
  return duration_cast<duration<double>>(high_resolution_clock::now() - start_time).count();
}