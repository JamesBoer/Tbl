/*
The Tbl library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See the LICENSE file or Tbl.hpp for license details.
Copyright (c) 2019 James Boer
*/

#include "UnitTest.h"
#include <fstream>
#include <string>
#include <sstream>

using namespace Tbl;


std::string LoadTestData(const char* filename)
{
#ifdef TBL_WINDOWS
	std::string filePath = std::string("../../../Data/") + std::string(filename);
#else
	std::string filePath = std::string("../../../../Data/" + std::string(filename);
#endif

	std::ifstream f(filePath);
	std::stringstream buffer;
	buffer << f.rdbuf();
	return buffer.str();
}

