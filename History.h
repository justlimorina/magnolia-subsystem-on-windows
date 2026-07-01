#pragma once
#include <string>
#include <vector>

class History {
public:
	explicit History(size_t maxEntries = 100);
	void add(const std::string& expr);
	std::string get(size_t index) const; // 0-based: 0 newest
	std::vector<std::string> list() const; // newest first
	size_t size() const;
private:
	size_t _max;
	std::vector<std::string> _entries; // newest first
};
