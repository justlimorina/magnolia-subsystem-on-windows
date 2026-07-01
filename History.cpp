#include "History.h"

History::History(size_t maxEntries) : _max(maxEntries) {}

void History::add(const std::string& expr) {
	if (expr.empty()) return;
	_entries.insert(_entries.begin(), expr);
	if (_entries.size() > _max) _entries.pop_back();
}

std::string History::get(size_t index) const {
	if (index >= _entries.size()) return std::string();
	return _entries[index];
}

std::vector<std::string> History::list() const {
	return _entries;
}

size_t History::size() const { return _entries.size(); }
