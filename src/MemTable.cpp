#include "MemTable.h"
#include "SSTable.h"
#include "Compression/ICompression.h"

namespace omx {

	void MemTable::log(SSTableRowPtr row) {
		if (m_wal) {
			m_wal->log(row);
		}
	}

	void MemTable::put(Key key, const std::string& value, const UInt128& checksum, EntryType entryType) {
		auto insertKey = InsertKey<Key>(m_counter++, key);

		auto row = std::make_shared<SSTableRow>(key, value, entryType, checksum);

		m_memorySize += row->getRowSize();

		log(row);
		m_map.insert({insertKey, std::move(row)});
	}

	void MemTable::put(Key key, const std::string& value, const UInt128& checksum) {
		put(key, value, checksum, EntryType::Put);
	}

	void MemTable::put(Key key, const std::string& value) {
		put(key, value, UInt128{});
	}

	void MemTable::remove(Key key) {
		put(key, "", UInt128{}, EntryType::Remove);
	}

	bool MemTable::get(Key key, std::string& value, UInt128& checksum) {
		auto it = m_map.find(SearchKey(key));

		if (it == m_map.end()) {
			return false;
		}

		auto row = it->second;

		if (row->getOperationType() == EntryType::Remove) {
			return false;
		}

		value = row->getData();
		checksum = row->getChecksum();

		return true;
	}

	bool MemTable::get(Key key, std::string& value) {
		UInt128 _;
		return get(key, value, _);
	}

	void MemTable::dump(std::ostream& os) {
		size_t offset = 0;
		size_t size = 0;

		size_t prevKeyId = std::string::npos;

		for (const auto& [insertKey, row]: m_map) {
			size_t keyId = insertKey.key.id;

			if (prevKeyId == keyId) {
				continue;
			}
			prevKeyId = keyId;

			auto data = serialize(row);

			os.write(data.data(), data.size());

			size = data.size();

			offset += size;
		}

		os.flush();
	}

	void MemTable::setWriteAheadLog(const std::string& path, uint32_t bufferSize) {
		m_wal = std::make_unique<WriteAheadLog>(path, bufferSize);
	}

	void MemTable::restoreFromLog(std::istream& stream) {
		if (stream.bad()) {
			throw std::runtime_error("bad input stream");
		}

		while (!stream.eof()) {
			auto row = deserialize(stream);

			auto key = InsertKey<Key>(m_counter++, row->getKey());

			m_map.insert({key, std::move(row)});

			stream.peek();
		}
	}

	size_t MemTable::getApproximateSize() const {
		return m_memorySize;
	}

	SSTable MemTable::createSortedStringsTable() const {
		auto table = SSTable();

		size_t prevKeyId = std::string::npos;

		for (const auto& [insertKey, row]: m_map) {
			size_t keyId = insertKey.key.id;

			if (prevKeyId == keyId) {
				continue;
			}
			prevKeyId = keyId;

			table.append(row);
		}

		return table;
	}

	SSTableIndexPtr MemTable::createSortedStringsTableIndex(size_t fileId) const {
		auto index = std::make_unique<SSTableIndex>(fileId);

		size_t offset = 0;
		size_t size = 0;
		size_t prevKeyId = std::string::npos;

		for (const auto& [insertKey, row]: m_map) {
			size_t keyId = insertKey.key.id;

			if (prevKeyId == keyId) {
				continue;
			}
			prevKeyId = keyId;

			size = row->getRowSize();
			index->insert(insertKey.key, FileSearchHint(offset, size));
			offset += size;
		}

		return index;
	}

	void MemTable::clear() {
		m_counter = 0;
		m_memorySize = 0;
		m_map.clear();
		m_wal->flushAndClear();
	}

	MemTable::MemTable() = default;
}
