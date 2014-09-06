#pragma once

#include <string>
#include <vector>

namespace String
{
	void ToUpper(std::string& str);
	void ToLower(std::string& str);

	void TrimLeft(std::string& str);
	void TrimRight(std::string& str);
	void Trim(std::string& str);

	bool Starts_With(const std::string& str, const std::string& prefix);
	bool Ends_With(const std::string& str, const std::string& suffix);
	bool Contains(const std::string& str, const std::string& containStr);


	std::vector<std::string> Split(const std::string& str);
}