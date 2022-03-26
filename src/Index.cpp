#include "Index.h"

namespace omx {

	std::size_t Index::Hasher::operator()(const Key& key) const {
		std::hash<std::size_t> hasher;
		return hasher(key.id);
	}

	void Index::insert(Key key, SearchHint hint) {
		m_map.insert_or_assign(key, hint);
	}

	void Index::merge(const Index& other) {
		for (const auto& [key, hint]: other.m_map) {
			insert(key, hint);
		}
	}

	bool Index::get(Key key, SearchHint& hint) {
		auto it = m_map.find(key);

		if (it == m_map.end()) {
			hint = SearchHint();
			return false;
		}

		hint = it->second;

		return true;
	}

	SearchHint::SearchHint(std::size_t fileId_, std::size_t offset_, std::size_t size_)
		: fileId(fileId_), offset(offset_), size(size_)
	{}
}
