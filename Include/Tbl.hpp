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

		size_t GetNumColumns() const { return m_numColumns; }
		size_t GetNumRows() const { return m_numRows; }
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
			assert(rowIndex < m_numRows);
			assert(columnIndex < m_numColumns);
			size_t index = columnIndex + (rowIndex * m_numColumns);
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

		void AdvanceToNextLine()
		{
			while (m_current != m_text.end())
			{
				const char c = *m_current;
				if (c == '\n' || c == '\r')
					++m_current;
				else
					break;
			}
		}

		bool DetectDelimiter()
		{
			size_t tabCount = 0;
			size_t commaCount = 0;
			auto current = m_text.begin();
			while (current != m_text.end())
			{
				const char c = *current;
				if (c == '\t')
					++tabCount;
				else if (c == ',')
					++commaCount;
				else if (c == '\n' || c == '\r')
					break;
				++current;
			}
			if (tabCount == commaCount)
				return false;
			m_delimiter = tabCount > commaCount ? '\t' : ',';
			return true;
		}

		bool ReadHeaderRow()
		{
			auto begin = m_text.begin();
			size_t index = 0;
			while (m_current != m_text.end())
			{
				const char c = *m_current;
				if (c == m_delimiter || c == '\n' || c == '\r')
				{
					String name(begin, m_current);
					m_columnMap.insert({name, index});
					begin = m_current;
					++index;
					if (c != m_delimiter)
						break;
					++m_current;
					begin = m_current;
				}
				else
					++m_current;
			}
			AdvanceToNextLine();
			m_numColumns = index;
			return !m_columnMap.empty();
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
        
		bool ReadRow()
		{
			// Read the row header (first column)
			size_t column = 0;
			auto begin = m_current;
			while (m_current != m_text.end())
			{
				const char c = *m_current;
				if (c == m_delimiter)
				{
					// Add the first column as a lookup for the row number
					String rowName(begin, m_current);
					m_rowMap.insert({ rowName, m_numRows });
					++m_numRows;
					++m_current;
					++column;
					begin = m_current;

					// We also store it as the first element of the row
					m_tableData.push_back(rowName);
					break;
				}
				else
					++m_current;
			}

			// Read the rest of the row
			while (m_current != m_text.end())
			{
				const char c = *m_current;
				if (c == m_delimiter || c == '\n' || c == '\r')
				{
					String str(begin, m_current);
					int64_t intValue = 0;
                    if (ParseInteger(str, intValue))
					{
						m_tableData.push_back(intValue);
					}
					else
					{
						double doubleValue = 0.0;
                        if (ParseDouble(str, doubleValue))
						{
							m_tableData.push_back(doubleValue);
						}
						else
						{
							m_tableData.push_back(str);
						}
					}
					if (c == m_delimiter)
					{
						++m_current;
						begin = m_current;
						++column;
						if (column > m_numColumns)
							return false;
					}
					else
					{
						AdvanceToNextLine();
						break;
					}
				}
				else
					++m_current;
			}

			return true;
		}

		bool ReadRows()
		{
			while (m_current != m_text.end())
			{
				if (!ReadRow())
					return false;
			}
			return true;
		}

		bool Read(std::string_view text)
		{
			m_text = text;
			m_current = m_text.begin();
			if (!DetectDelimiter())
				return false;
			if (!ReadHeaderRow())
				return false;
			if (!ReadRows())
				return false;
			return true;
		}

		std::string_view m_text;
		std::string_view::const_iterator m_current;
		VectorTableData m_tableData;
		StringIndexMap m_columnMap;
		StringIndexMap m_rowMap;
		bool m_error = false;
		size_t m_numColumns = 0;
		size_t m_numRows = 0;
		char m_delimiter = 0;

	};
	
}


