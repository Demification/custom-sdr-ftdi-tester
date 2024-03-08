#pragma once
#include <string>
#include <utility>
#include <iostream>

#ifdef DEBUG
#define __DEBUG_ERROR__(X)  std::cerr << "\033[31m" << __FILE__ << " " << __PRETTY_FUNCTION__ << " Line:" <<__LINE__ << " Error: " << X << "\033[0m" << std::endl
#define __DEBUG_INFO__(X)   std::cerr << "\033[32m" << X << "\033[0m" << std::endl
#else
#define __DEBUG_ERROR__(X)  std::cerr << X << std::endl
#define __DEBUG_INFO__(X)   std::cerr << X << std::endl
#endif