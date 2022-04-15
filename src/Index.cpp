#include "Index.h"

namespace omx {

	void Index::insert(Key key, SearchHint hint) {
		m_map.insert_or_assign(key, hint);
	}

	void Index::merge(const Index& other) {
		for (const auto& [key, hint]: other.m_map) {
			m_map.insert_or_assign(key, hint);
		}
	}

	bool Index::get(Key key, SearchHint& hint) const {
		auto it = m_map.find(key);

		if (it == m_map.end()) {
			hint = SearchHint();
			return false;
		}

		hint = it->second;

		return true;
	}

	void Index::dump(std::ostream& stream) {
		constexpr size_t sizeOfKey = sizeof(Key::id);
		constexpr size_t sizeOfHint = sizeof(SearchHint);

		for (const auto& [key, hint]: m_map) {
			stream.write(reinterpret_cast<const char*>(&sizeOfKey), sizeof(sizeOfKey));
			stream.write(reinterpret_cast<const char*>(&key.id), sizeOfKey);
			stream.write(reinterpret_cast<const char*>(&sizeOfHint), sizeof(sizeOfHint));
			stream.write(reinterpret_cast<const char*>(&hint), sizeOfHint);
		}

		stream.flush();
	}

	void Index::load(std::istream& stream) {
		size_t sizeOfKey = 0;
		size_t sizeOfHint = 0;
		Key key;
		SearchHint hint;

		while (!stream.eof()) {
			stream.read(reinterpret_cast<char*>(&sizeOfKey), sizeof(sizeOfKey));
			stream.read(reinterpret_cast<char*>(&key.id), sizeOfKey);
			stream.read(reinterpret_cast<char*>(&sizeOfHint), sizeof(sizeOfHint));
			stream.read(reinterpret_cast<char*>(&hint), sizeOfHint);
			stream.peek();
			m_map.insert({key, hint});
		}
	}

	SearchHint::SearchHint(std::size_t fileId_, std::size_t offset_, std::size_t size_)
		: fileId(fileId_), offset(offset_), size(size_)
	{}
}
