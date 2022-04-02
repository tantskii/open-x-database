#include "Index.h"

namespace omx {

	std::size_t Index::Hasher::operator()(const Key& key) const {
		std::hash<std::size_t> hasher;
		return hasher(key.id);
	}

	void Index::insert(Key key, SearchHint hint) {
		std::unique_lock lock(m_mutex);
		m_map.insert_or_assign(key, hint);
	}

	void Index::merge(const Index& other) {
		std::unique_lock lock(m_mutex);
		for (const auto& [key, hint]: other.m_map) {
			m_map.insert_or_assign(key, hint);
		}
	}

	bool Index::get(Key key, SearchHint& hint) const {
		std::shared_lock lock(m_mutex);
		auto it = m_map.find(key);

		if (it == m_map.end()) {
			hint = SearchHint();
			return false;
		}

		hint = it->second;

		return true;
	}

	void Index::dump(std::ostream& stream) {
		std::unique_lock lock(m_mutex);

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
		std::unique_lock lock(m_mutex);

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
