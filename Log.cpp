/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Log helper
 */

#include <iostream>

#include "Log.hpp"

using namespace kF;

Core::Log Core::InfoLog(std::cout);
Core::Log Core::ErrorLog(std::cerr);