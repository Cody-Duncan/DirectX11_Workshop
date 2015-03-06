#include "StringAlgorithm.h"
#include <algorithm>
#include <cctype>

namespace String
{

	void ToUpper(std::string& str)
	{
		std::string out;
		std::transform(str.begin(), str.end(), std::back_inserter(out), std::toupper);
		str.swap(out);
	}

	void ToLower(std::string& str)
	{
		std::string out;
		std::transform(str.begin(), str.end(), std::back_inserter(out), std::tolower);
		str.swap(out);
	}


	void TrimLeft(std::string& str)
	{
		unsigned int loc = str.find_first_not_of(" \t\r\n");
		str = str.substr(loc);
	}

	void TrimRight(std::string& str)
	{
		unsigned int loc = str.find_last_not_of(" \t\r\n");
		str = str.substr(0, loc + 1);
	}

	void Trim(std::string& str)
	{
		unsigned int start = str.find_first_not_of(" \t\r\n");
		unsigned int end = str.find_last_not_of(" \t\r\n");
		str = str.substr(start, end - start + 1);
	}


	bool Starts_With(const std::string& str, const std::string& prefix)
	{
		return str.size() >= prefix.size() &&
			(str.compare(0, prefix.size(), prefix) == 0);
	}

	bool Ends_With(const std::string& str, const std::string& suffix)
	{
		return str.size() >= suffix.size() &&
			(str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0);
	}

	bool Contains(const std::string& str, const std::string& containStr)
	{
		return str.find(containStr) != std::string::npos;
	}


	template < class ContainerT >
	void tokenize(const std::string& str, ContainerT& tokens, const std::string& delimiters = " ", bool trimEmpty = false)
	{
		std::string::size_type pos;
		std::string::size_type lastPos = 0;
		while (true)
		{
			pos = str.find_first_of(delimiters, lastPos);

			auto AddToTokens = [&]()
			{
				//if the position moved, or we are not trimming empty space, add the token
				if (pos != lastPos || !trimEmpty)
				{
					tokens.emplace_back(str.data() + lastPos, (ContainerT::value_type::size_type)pos - lastPos);
				}
			};

			if (pos == std::string::npos)  //could not find next delimeter
			{
				pos = str.length();

				AddToTokens();
				break; //exit
			}

			AddToTokens();

			lastPos = pos + 1;
		}
	};

	std::vector<std::string> Split(const std::string& str)
	{
		std::vector<std::string> tokens;
		tokenize(str, tokens);
		return std::move(tokens);
	}
	
	std::string SubstringAfter(std::string& str, std::string matchChars)
	{
		size_t loc = str.find_last_of(matchChars);
		if (loc != std::string::npos)
		{
			return str.substr(loc);
		}
		
		return std::string();
	}
}