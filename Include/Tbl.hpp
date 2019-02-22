/*
The MIT License (MIT)

Copyright (c) 2019 James Boer

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#pragma once

// Platform definitions
#if defined(_WIN32) || defined(_WIN64)
#define TBL_WINDOWS
#define USE_FROM_CHARS
#pragma warning(push)
#pragma warning(disable : 4530) // Silence warnings if exceptions are disabled
#endif

#if defined(__linux__) || defined(__linux)
#define TBL_LINUX
#endif

#ifdef __APPLE__
#ifdef __MACH__
#define TBL_MACOS
#endif
#endif

#include <cassert>
#include <variant>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#ifdef USE_FROM_CHARS
#include <charconv>
#else
#include <cstdlib>
#endif

#ifdef TBL_WINDOWS
#pragma warning(pop)
#endif

namespace Tbl
{

	template<typename Alloc = std::allocator<char>>
	class Table
	{
	public:
		using String = std::basic_string<char, std::char_traits<char>, Alloc>;
		using TableData = std::variant<int64_t, double, String>;
	private:
		using TableDataAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<TableData>;
		using VectorTableData = std::vector<TableData, TableDataAlloc>;
		using StringIndexPair = std::pair<const String, size_t>;
		using StringIndexPairAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<StringIndexPair>;
		using StringIndexMap = std::unordered_map<const String, size_t, std::hash<String>, std::equal_to<String>, StringIndexPairAlloc>;
	public:

		Table(std::string_view text)
		{
			m_error = !Read(text);
		}

		explicit operator bool() const { return !m_error; }

		size_t GetNumColumns() const { return m_columnMap.size(); }
		size_t GetNumRows() const { return m_rowMap.size(); }
		size_t GetRowIndex(const String& rowName) const
		{
			const auto& pair = m_rowMap.find(rowName);
			assert(pair != m_rowMap.end());
			return pair->second;
		}
		size_t GetColumnIndex(const String& columnName) const
		{
			const auto& pair = m_columnMap.find(columnName);
			assert(pair != m_columnMap.end());
			return pair->second;
		}
		TableData GetData(size_t rowIndex, size_t columnIndex)
		{
			assert(!m_error);
			assert(rowIndex < GetNumRows());
			assert(columnIndex < GetNumColumns());
			size_t index = columnIndex + (rowIndex * GetNumColumns());
			assert(index < m_tableData.size());
			return m_tableData[index];
		}
		TableData GetData(const String& rowName, const String& columnName)
		{
			return GetData(GetRowIndex(rowName), GetColumnIndex(columnName));
		}
		template <typename T>
		T Get(size_t rowIndex, size_t columnIndex)
		{
			return std::get<T>(GetData(rowIndex, columnIndex));
		}
		template <typename T>
		T Get(const String& rowName, const String& columnName)
		{
			return std::get<T>(GetData(GetRowIndex(rowName), GetColumnIndex(columnName)));
		}

	private:

		bool IsLineEnd(std::string_view::const_iterator current) const
		{
			return *current == '\n' || *current == '\r';
		}

		void AdvanceToNextLine(std::string_view text, std::string_view::const_iterator& current) const
		{
			while (current != text.end())
			{
				if (!IsLineEnd(current))
					break;
				++current;
			}
		}

		bool DetectDelimiter(std::string_view text, char& delimiter) const
		{
			size_t tabCount = 0;
			size_t commaCount = 0;
			auto current = text.begin();
			while (current != text.end())
			{
				const char c = *current;
				if (c == '\t')
					++tabCount;
				else if (c == ',')
					++commaCount;
				else if (IsLineEnd(current))
					break;
				++current;
			}
			if (tabCount == commaCount)
				return false;
			delimiter = tabCount > commaCount ? '\t' : ',';
			return true;
		}

        bool ParseInteger(const String& str, int64_t& intValue)
        {
#ifdef USE_FROM_CHARS
            auto result = std::from_chars(str.data(), str.data() + str.size(), intValue);
            if (result.ptr == str.data() + str.size())
                return true;
            return false;
#else
            char * endPtr;
            intValue = strtoll(str.data(), &endPtr, 10);
            if (endPtr != (str.data() + str.size()))
                return false;
            return true;
#endif
        }

        bool ParseDouble(const String& str, double& doubleValue)
        {
#ifdef USE_FROM_CHARS
            auto result = std::from_chars(str.data(), str.data() + str.size(), doubleValue);
            if (result.ptr == str.data() + str.size())
                return true;
            return false;
#else
            char * endPtr;
            doubleValue = strtod(str.data(), &endPtr);
            if (endPtr != (str.data() + str.size()))
                return false;
            return true;
#endif
        }

		TableData ParseData(const String& str)
		{
			int64_t intValue = 0;
			if (ParseInteger(str, intValue))
				return intValue;
			else
			{
				double doubleValue = 0.0;
				if (ParseDouble(str, doubleValue))
					return doubleValue;
			}
			return str;
		}

		String ParseCell(std::string_view text, char delimiter, std::string_view::const_iterator& current)
		{
			// First check if this cell is double-quoted.  If so, we ignore
			// delimiters until another stand-alone double-quote is found.
			// Note that double-quotes can be "escaped" with a pair of double-quotes
			// as well, so after the first quote, we check for that too.

			//bool quoted = *current == '"';

			String str;
			while (current != text.end())
			{
				// Check for breaking delimiters
				const char c = *current;
				if (c == delimiter || IsLineEnd(current))
					break;
				str += c;
				++current;
			}
			return str;
		}

		bool ReadHeader(std::string_view text, std::string_view::const_iterator& current, char delimiter)
		{
			while (current != text.end())
			{
				auto str = ParseCell(text, delimiter, current);
				m_columnMap.insert({ str, m_columnMap.size() });
				if (IsLineEnd(current))
					break;
				++current;
			}
			AdvanceToNextLine(text, current);
			return true;
		}

		bool ReadRow(std::string_view text, std::string_view::const_iterator& current, char delimiter)
		{
			// Track column data
			size_t column = 0;

			// Make sure we don't parse past the end of the string data
			while (current != text.end())
			{
				auto str = ParseCell(text, delimiter, current);
				if (column == 0)
				{
					m_rowMap.insert({ str, m_rowMap.size() });
					m_tableData.push_back(str);
				}
				else
					m_tableData.push_back(ParseData(str));
				++column;
				if (current == text.end() || IsLineEnd(current))
					break;
				++current;
			}
			AdvanceToNextLine(text, current);
			return column == GetNumColumns();
		}

		bool ReadRows(std::string_view text, std::string_view::const_iterator& current, char delimiter)
		{
			while (current != text.end())
			{
				if (!ReadRow(text, current, delimiter))
					return false;
			}
			return true;
		}

		bool Read(std::string_view text)
		{
			char delimiter = 0;
			if (!DetectDelimiter(text, delimiter))
				return false;
			std::string_view::const_iterator current = text.begin();
			if (!ReadHeader(text, current, delimiter))
				return false;
			if (!ReadRows(text, current, delimiter))
				return false;
			return true;
		}

		VectorTableData m_tableData;
		StringIndexMap m_columnMap;
		StringIndexMap m_rowMap;
		bool m_error = false;
	};

}


