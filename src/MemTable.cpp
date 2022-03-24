#include "MemTable.h"

namespace omx {

	bool MemTable::Comparator::operator()(const Key& lhs, const Key& rhs) const {
		return lhs.id < rhs.id;
	}

	void MemTable::put(Key key, const Bytes& value) {
		std::unique_lock lock(m_mutex);

		auto insertKey = InsertKey<Key, Comparator>(m_counter++, key);
		auto entry = Entry(key, value, Operation::Put);

		m_map.insert_or_assign(insertKey, entry);
	}

	void MemTable::remove(Key key) {
		std::unique_lock lock(m_mutex);

		auto insertKey = InsertKey<Key, Comparator>(m_counter++, key);
		auto entry = Entry(key);

		m_map.insert_or_assign(insertKey, entry);
	}

	void MemTable::get(Key key, Bytes& value) {
		std::shared_lock lock(m_mutex);

		auto searchKey = SearchKey<Key>(key);

		auto it = m_map.find(searchKey);

		if (it == m_map.end()) {
			return;
		}

		const auto& entry = it->second;

		if (entry.getOperationType() == Operation::Remove) {
			return;
		}

		value.toString() = entry.getBytes().toString();
	}
}
