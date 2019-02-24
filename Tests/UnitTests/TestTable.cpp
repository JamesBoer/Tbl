/*
The Tbl library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See the LICENSE file or Tbl.hpp for license details.
Copyright (c) 2019 James Boer
*/

#include "UnitTest.h"
#include <clocale>

using namespace Tbl;
using String = Tbl::Table<>::String;

TEST_CASE("Test Tables", "[Tables]")
{

    SECTION("Tab-Delimited Table Test")
    {
		static const char * tableText =
			"Name Field\tInteger Field\tFloat Field\tText Field\n"
			"Test Name A\t1\t4.5\tThis is a simple test.\n"
			"Test Name B\t2\t123.456\tMore to test...\n"
			"Test Name C\t3\t22.3345\tEven more tests of text\n"
			;

		Table t(tableText);
        REQUIRE(t);
		REQUIRE(t.GetNumColumns() == 4);
		REQUIRE(t.GetNumRows() == 3);

		REQUIRE(t.Get<String>(0, 0) == "Test Name A");
		REQUIRE(t.Get<int64_t>(0, 1) == 1);
		REQUIRE(t.Get<double>(0, 2) == Approx(4.5));
		REQUIRE(t.Get<String>(0, 3) == "This is a simple test.");

		REQUIRE(t.Get<String>(1, 0) == "Test Name B");
		REQUIRE(t.Get<int64_t>(1, 1) == 2);
		REQUIRE(t.Get<double>(1, 2) == Approx(123.456));
		REQUIRE(t.Get<String>(1, 3) == "More to test...");

		REQUIRE(t.Get<String>(2, 0) == "Test Name C");
		REQUIRE(t.Get<int64_t>(2, 1) == 3);
		REQUIRE(t.Get<double>(2, 2) == Approx(22.3345));
		REQUIRE(t.Get<String>(2, 3) == "Even more tests of text");

		REQUIRE(t.Get<String>("Test Name A", "Name Field") == "Test Name A");
		REQUIRE(t.Get<int64_t>("Test Name A", "Integer Field") == 1);
		REQUIRE(t.Get<double>("Test Name A", "Float Field") == Approx(4.5));
		REQUIRE(t.Get<String>("Test Name A", "Text Field") == "This is a simple test.");

		REQUIRE(t.Get<String>("Test Name B", "Name Field") == "Test Name B");
		REQUIRE(t.Get<int64_t>("Test Name B", "Integer Field") == 2);
		REQUIRE(t.Get<double>("Test Name B", "Float Field") == Approx(123.456));
		REQUIRE(t.Get<String>("Test Name B", "Text Field") == "More to test...");

		REQUIRE(t.Get<String>("Test Name C", "Name Field") == "Test Name C");
		REQUIRE(t.Get<int64_t>("Test Name C", "Integer Field") == 3);
		REQUIRE(t.Get<double>("Test Name C", "Float Field") == Approx(22.3345));
		REQUIRE(t.Get<String>("Test Name C", "Text Field") == "Even more tests of text");
	}
	
	SECTION("Comma-Delimited Table Test")
	{
		static const char * tableText =
			"Name Field,Integer Field,Float Field,Text Field\n"
			"Test Name A,1,4.5,This is a simple test.\n"
			"Test Name B,2,123.456,More to test...\n"
			"Test Name C,3,22.3345,Even more tests of text\n"
			;

		Table t(tableText);
		REQUIRE(t);
		REQUIRE(t.GetNumColumns() == 4);
		REQUIRE(t.GetNumRows() == 3);

		REQUIRE(t.Get<String>(0, 0) == "Test Name A");
		REQUIRE(t.Get<int64_t>(0, 1) == 1);
		REQUIRE(t.Get<double>(0, 2) == Approx(4.5));
		REQUIRE(t.Get<String>(0, 3) == "This is a simple test.");

		REQUIRE(t.Get<String>(1, 0) == "Test Name B");
		REQUIRE(t.Get<int64_t>(1, 1) == 2);
		REQUIRE(t.Get<double>(1, 2) == Approx(123.456));
		REQUIRE(t.Get<String>(1, 3) == "More to test...");

		REQUIRE(t.Get<String>(2, 0) == "Test Name C");
		REQUIRE(t.Get<int64_t>(2, 1) == 3);
		REQUIRE(t.Get<double>(2, 2) == Approx(22.3345));
		REQUIRE(t.Get<String>(2, 3) == "Even more tests of text");

		REQUIRE(t.Get<String>("Test Name A", "Name Field") == "Test Name A");
		REQUIRE(t.Get<int64_t>("Test Name A", "Integer Field") == 1);
		REQUIRE(t.Get<double>("Test Name A", "Float Field") == Approx(4.5));
		REQUIRE(t.Get<String>("Test Name A", "Text Field") == "This is a simple test.");

		REQUIRE(t.Get<String>("Test Name B", "Name Field") == "Test Name B");
		REQUIRE(t.Get<int64_t>("Test Name B", "Integer Field") == 2);
		REQUIRE(t.Get<double>("Test Name B", "Float Field") == Approx(123.456));
		REQUIRE(t.Get<String>("Test Name B", "Text Field") == "More to test...");

		REQUIRE(t.Get<String>("Test Name C", "Name Field") == "Test Name C");
		REQUIRE(t.Get<int64_t>("Test Name C", "Integer Field") == 3);
		REQUIRE(t.Get<double>("Test Name C", "Float Field") == Approx(22.3345));
		REQUIRE(t.Get<String>("Test Name C", "Text Field") == "Even more tests of text");
	}

	SECTION("Continental Format Table Test")
	{
		static const char * tableText =
			"Name Field;Integer Field;Float Field;Text Field\n"
			"Test Name A;1;4,5;This is a simple test.\n"
			"Test Name B;2;123,456;More to test...\n"
			"Test Name C;3;22,3345;Even more tests of text\n"
			;

		Table t(tableText);
		REQUIRE(t);
		REQUIRE(t.GetNumColumns() == 4);
		REQUIRE(t.GetNumRows() == 3);

		REQUIRE(t.Get<String>(0, 0) == "Test Name A");
		REQUIRE(t.Get<int64_t>(0, 1) == 1);
		REQUIRE(t.Get<double>(0, 2) == Approx(4.5));
		REQUIRE(t.Get<String>(0, 3) == "This is a simple test.");

		REQUIRE(t.Get<String>(1, 0) == "Test Name B");
		REQUIRE(t.Get<int64_t>(1, 1) == 2);
		REQUIRE(t.Get<double>(1, 2) == Approx(123.456));
		REQUIRE(t.Get<String>(1, 3) == "More to test...");

		REQUIRE(t.Get<String>(2, 0) == "Test Name C");
		REQUIRE(t.Get<int64_t>(2, 1) == 3);
		REQUIRE(t.Get<double>(2, 2) == Approx(22.3345));
		REQUIRE(t.Get<String>(2, 3) == "Even more tests of text");

		REQUIRE(t.Get<String>("Test Name A", "Name Field") == "Test Name A");
		REQUIRE(t.Get<int64_t>("Test Name A", "Integer Field") == 1);
		REQUIRE(t.Get<double>("Test Name A", "Float Field") == Approx(4.5));
		REQUIRE(t.Get<String>("Test Name A", "Text Field") == "This is a simple test.");

		REQUIRE(t.Get<String>("Test Name B", "Name Field") == "Test Name B");
		REQUIRE(t.Get<int64_t>("Test Name B", "Integer Field") == 2);
		REQUIRE(t.Get<double>("Test Name B", "Float Field") == Approx(123.456));
		REQUIRE(t.Get<String>("Test Name B", "Text Field") == "More to test...");

		REQUIRE(t.Get<String>("Test Name C", "Name Field") == "Test Name C");
		REQUIRE(t.Get<int64_t>("Test Name C", "Integer Field") == 3);
		REQUIRE(t.Get<double>("Test Name C", "Float Field") == Approx(22.3345));
		REQUIRE(t.Get<String>("Test Name C", "Text Field") == "Even more tests of text");
	}

	SECTION("Test Table No End-Line")
	{
		static const char * tableText =
			"Name Field\tText Field\n"
			"Test Name\tDo you read me?"
			;

		Table t(tableText);
		REQUIRE(t);
		REQUIRE(t.GetNumColumns() == 2);
		REQUIRE(t.GetNumRows() == 1);
		REQUIRE(t.Get<String>("Test Name", "Text Field") == "Do you read me?");
	}

    SECTION("Comma-Delimited Table With Commas")
    {
		static const char * tableText =
			"Name Field,Text Field\n"
			"Test Name A,\"Test 1, 2, 3\"\n"
			"Test Name B,Test 4\n"
			;

		Table t(tableText);
        REQUIRE(t);
		REQUIRE(t.GetNumColumns() == 2);
		REQUIRE(t.GetNumRows() == 2);
		REQUIRE(t.Get<String>("Test Name A", "Text Field") == "Test 1, 2, 3");
		REQUIRE(t.Get<String>("Test Name B", "Text Field") == "Test 4");
	}

	SECTION("Comma-Delimited Table With Quotes")
	{
		static const char * tableText =
			"Name Field,Text Field\n"
			"Test Name A,\"\"\"Quoted text\"\"\"\n"
			;

		Table t(tableText);
		REQUIRE(t);
		REQUIRE(t.GetNumColumns() == 2);
		REQUIRE(t.GetNumRows() == 1);
		REQUIRE(t.Get<String>("Test Name A", "Text Field") == "\"Quoted text\"");
	}

	SECTION("Comma-Delimited Table With Quotes and No Ending Newline")
	{
		static const char * tableText =
			"Name Field,Text Field\n"
			"Test Name A,\"\"\"Quoted text\"\"\""
			;

		Table t(tableText);
		REQUIRE(t);
		REQUIRE(t.GetNumColumns() == 2);
		REQUIRE(t.GetNumRows() == 1);
		REQUIRE(t.Get<String>("Test Name A", "Text Field") == "\"Quoted text\"");
	}

	SECTION("Tab-Delimited Table Test From File 1")
	{
#ifdef TBL_WINDOWS
		auto tableText = LoadTestData("../../../Data/Test1.txt");
#else
		auto tableText = LoadTestData("../../../../Data/Test1.txt");
#endif
		Table t(tableText);
		REQUIRE(t);
		REQUIRE(t.GetNumColumns() == 4);
		REQUIRE(t.GetNumRows() == 3);
	}

	SECTION("Comma-Delimited Table Test From File 1")
	{
#ifdef TBL_WINDOWS
		auto tableText = LoadTestData("../../../Data/Test1.csv");
#else
		auto tableText = LoadTestData("../../../../Data/Test1.csv");
#endif
		Table t(tableText);
		REQUIRE(t);
		REQUIRE(t.GetNumColumns() == 4);
		REQUIRE(t.GetNumRows() == 3);
	}

	SECTION("Tab-Delimited Table Test From File 2")
	{
#ifdef TBL_WINDOWS
		auto tableText = LoadTestData("../../../Data/Test2.txt");
#else
		auto tableText = LoadTestData("../../../../Data/Test2.txt");
#endif
		Table t(tableText);
		REQUIRE(t);
		REQUIRE(t.GetNumColumns() == 2);
		REQUIRE(t.GetNumRows() == 3);
		REQUIRE(t.Get<String>("Test Name A", "Text Field") == "Test 1, Test 2, Test 3");
		REQUIRE(t.Get<String>("Test Name B", "Text Field") == "\"Quoted text\"");
		REQUIRE(t.Get<String>("Test Name C", "Text Field") == "Try \"A, B, C\" for fun!");
	}

	SECTION("Comma-Delimited Table Test From File 2")
	{
#ifdef TBL_WINDOWS
		auto tableText = LoadTestData("../../../Data/Test2.csv");
#else
		auto tableText = LoadTestData("../../../../Data/Test2.csv");
#endif
		Table t(tableText);
		REQUIRE(t);
		REQUIRE(t.GetNumColumns() == 2);
		REQUIRE(t.GetNumRows() == 3);
		REQUIRE(t.Get<String>("Test Name A", "Text Field") == "Test 1, Test 2, Test 3");
		REQUIRE(t.Get<String>("Test Name B", "Text Field") == "\"Quoted text\"");
		REQUIRE(t.Get<String>("Test Name C", "Text Field") == "Try \"A, B, C\" for fun!");
	}

	SECTION("Semicolon-Delimited Table Test From File 3")
	{
#ifdef TBL_WINDOWS
		auto tableText = LoadTestData("../../../Data/Test3.csv");
#else
		auto tableText = LoadTestData("../../../../Data/Test3.csv");
#endif
		Table t(tableText);
		REQUIRE(t);
		REQUIRE(t.GetNumColumns() == 4);
		REQUIRE(t.GetNumRows() == 3);
		REQUIRE(t.Get<double>("Test Name A", "Float Field") == Approx(4.5));
		REQUIRE(t.Get<double>("Test Name B", "Float Field") == Approx(123.456));
		REQUIRE(t.Get<double>("Test Name C", "Float Field") == Approx(22.3345));
	}

	SECTION("Comma-Delimited Table Test with German Locale")
	{
		static const char * tableText =
			"Name Field,Integer Field,Float Field,Text Field\n"
			"Test Name A,1,4.5,This is a simple test.\n"
			"Test Name B,2,123.456,More to test...\n"
			"Test Name C,3,22.3345,Even more tests of text\n"
			;
		std::setlocale(LC_NUMERIC, "de_DE.UTF-8");
		Table t(tableText);
		std::setlocale(LC_NUMERIC, "en_US.UTF-8");
		REQUIRE(t);
		REQUIRE(t.GetNumColumns() == 4);
		REQUIRE(t.GetNumRows() == 3);
		REQUIRE(t.Get<double>("Test Name A", "Float Field") == Approx(4.5));
		REQUIRE(t.Get<double>("Test Name B", "Float Field") == Approx(123.456));
		REQUIRE(t.Get<double>("Test Name C", "Float Field") == Approx(22.3345));
	}

}
