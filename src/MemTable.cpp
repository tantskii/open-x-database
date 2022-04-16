#include "MemTable.h"
#include "SSTable.h"
#include "Compression/ICompression.h"
#include "Logging/Logging.h"

namespace omx {

	void MemTable::log(SSTableRowPtr row) {
		if (m_wal) {
			m_wal->log(row);
		}
	}

	void MemTable::put(Key key, const std::string& value, const UInt128& checksum) {
		if (m_isImmutable) {
			return;
		}

		auto insertKey = InsertKey<Key>(m_counter++, key);

		auto row = std::make_shared<SSTableRow>(key, value, checksum);

		m_memorySize += row->getRowSize();

		log(row);
		m_map.insert({insertKey, std::move(row)});
	}

	void MemTable::put(Key key, const std::string& value) {
		put(key, value, UInt128{});
	}

	void MemTable::remove(Key key) {
		if (m_isImmutable) {
			return;
		}

		auto insertKey = InsertKey<Key>(m_counter++, key);
		auto row = std::make_shared<SSTableRow>(key);
		m_memorySize += row->getRowSize();

		log(row);
		m_map.insert({insertKey, std::move(row)});
	}

	bool MemTable::get(Key key, std::string& value, UInt128& checksum) {
		if (m_isImmutable) {
			return false;
		}

		auto searchKey = SearchKey<Key>(key);

		auto it = m_map.find(searchKey);

		if (it == m_map.end()) {
			return false;
		}

		const auto& row = it->second;

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
		m_isImmutable = true;

		size_t offset = 0;
		size_t size = 0;

		size_t prevKeyId = std::string::npos;

		for (const auto& [insertKey, row]: m_map) {
			size_t keyId = insertKey.key.id;

			if (prevKeyId == keyId) {
				continue;
			}
			prevKeyId = keyId;

			std::string data = serialize(row);

			os.write(data.data(), data.size());

			size = data.size();

			offset += size;
		}

		os.flush();
	}

	void MemTable::setWriteAheadLog(const std::string& path) {
		m_wal = std::make_unique<WriteAheadLog>(path);
	}

	void MemTable::restoreFromLog(std::istream& stream) {
		if (stream.bad()) {
			log::error("[%s] bad input stream", __PRETTY_FUNCTION__);
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
		SSTable table;

		for (const auto& [_, row]: m_map) {
			table.append(row);
		}

		return table;
	}

	Index MemTable::createIndex(const size_t fileId) const {
		Index index;
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
			index.insert(insertKey.key, SearchHint(fileId, offset, size));
			offset += size;
		}

		return std::move(index);
	}


	MemTable::MemTable() = default;
}
