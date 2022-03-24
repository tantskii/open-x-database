#include "MemTable.h"

namespace omx {

	bool MemTable::Comparator::operator()(const Key& lhs, const Key& rhs) const {
		return lhs.id < rhs.id;
	}

	void MemTable::put(Key key, const Bytes& value) {
		std::unique_lock lock(m_mutex);

		m_map.insert_or_assign(key, value);
	}

	void MemTable::remove(Key key) {
		std::unique_lock lock(m_mutex);

		m_map.erase(key);
	}

	void MemTable::get(Key key, Bytes& value) {
		std::shared_lock lock(m_mutex);

		auto it = m_map.find(key);

		if (it == m_map.end()) {
			return;
		}

		value = it->second;
	}
}
