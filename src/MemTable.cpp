#include "MemTable.h"

namespace omx {

	bool MemTable::Comparator::operator()(const Key& lhs, const Key& rhs) const {
		return lhs.id < rhs.id;
	}

	void MemTable::log(const Entry& entry) {
		if (m_wal) {
			m_wal->log(entry);
		}
	}

	void MemTable::put(Key key, const std::string& value) {
		std::unique_lock lock(m_mutex);

		if (m_isClosed) {
			return;
		}

		auto insertKey = InsertKey<Key, Comparator>(m_counter++, key);
		auto entry = Entry(key, value);

		log(entry);
		m_map.insert({insertKey, std::move(entry)});
	}

	void MemTable::remove(Key key) {
		std::unique_lock lock(m_mutex);

		if (m_isClosed) {
			return;
		}

		auto insertKey = InsertKey<Key, Comparator>(m_counter++, key);
		auto entry = Entry(key);

		log(entry);
		m_map.insert({insertKey, std::move(entry)});
	}

	bool MemTable::get(Key key, std::string& value) {
		std::shared_lock lock(m_mutex);

		if (m_isClosed) {
			return false;
		}

		auto searchKey = SearchKey<Key>(key);

		auto it = m_map.find(searchKey);

		if (it == m_map.end()) {
			return false;
		}

		const auto& entry = it->second;

		if (entry.getOperationType() == EntryType::Remove) {
			return false;
		}

		value = entry.getData();

		return true;
	}

	void MemTable::dump(size_t fileId, std::ostream& os, Index& index) {
		std::unique_lock lock(m_mutex);

		m_isClosed = true;

		size_t offset = 0;
		size_t size = 0;

		size_t prevKeyId = std::string::npos;

		for (const auto& [insertKey, value]: m_map) {
			size_t keyId = insertKey.key.id;

			if (prevKeyId == keyId) {
				continue;
			}
			prevKeyId = keyId;

			size = value.serialize(os);
			index.insert(insertKey.key, SearchHint(fileId, offset, size));
			offset += size;
		}
	}

	void MemTable::setWriteAheadLog(std::ostream& stream) {
		m_wal = std::make_unique<WriteAheadLog>(stream);
	}

	void MemTable::restoreFromLog(std::istream& stream) {
		std::unique_lock lock(m_mutex);

		Entry entry;
		while (!stream.eof()) {
			entry.deserialize(stream);

			auto key = entry.getKey();
			auto insertKey = InsertKey<Key, Comparator>(m_counter++, key);
			m_map.insert({insertKey, entry});
		}
	}
}
