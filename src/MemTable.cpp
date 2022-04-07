#include "MemTable.h"
#include "SSTable.h"
#include "Compression/ICompression.h"

namespace omx {

	void MemTable::log(SSTableRowPtr row) {
		if (m_wal) {
			m_wal->log(row);
		}
	}

	void MemTable::put(Key key, const std::string& value) {
		std::unique_lock lock(m_mutex);

		if (m_isImmutable) {
			return;
		}

		auto insertKey = InsertKey<Key>(m_counter++, key);
		auto row = std::make_shared<SSTableRow>(key, value);
		m_memorySize += row->getRowSize();

		log(row);
		m_map.insert({insertKey, std::move(row)});
	}

	void MemTable::remove(Key key) {
		std::unique_lock lock(m_mutex);

		if (m_isImmutable) {
			return;
		}

		auto insertKey = InsertKey<Key>(m_counter++, key);
		auto row = std::make_shared<SSTableRow>(key);
		m_memorySize += row->getRowSize();

		log(row);
		m_map.insert({insertKey, std::move(row)});
	}

	bool MemTable::get(Key key, std::string& value) {
		std::shared_lock lock(m_mutex);

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

		return true;
	}

	void MemTable::dump(size_t fileId, std::ostream& os, Index& index) {
		std::unique_lock lock(m_mutex);

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

			std::string uncompressed = serialize(row);
			std::string compressed;
			m_compressor->compress(uncompressed, compressed);

			os.write(compressed.data(), compressed.size());

			UInt128 checksum = m_hasher->hash(compressed);

			os.write(reinterpret_cast<const char*>(&checksum.first), sizeof(checksum));

			size = compressed.size() + sizeof(checksum);

			index.insert(insertKey.key, SearchHint(fileId, offset, size));

			offset += size;
		}

		os.flush();
	}

	void MemTable::setWriteAheadLog(const std::string& path) {
		std::unique_lock lock(m_mutex);
		m_wal = std::make_unique<WriteAheadLog>(path);
	}

	void MemTable::restoreFromLog(std::istream& stream) {
		std::unique_lock lock(m_mutex);

		while (!stream.eof()) {
			auto row = deserialize(stream);

			auto key = row->getKey();
			auto insertKey = InsertKey<Key>(m_counter++, key);
			m_map.insert({insertKey, std::move(row)});

			stream.peek();
		}
	}

	size_t MemTable::getApproximateSize() const {
		std::shared_lock lock(m_mutex);
		return m_memorySize;
	}

	SSTable MemTable::createSortedStringsTable() const {
		std::shared_lock lock(m_mutex);

		SSTable table;

		for (const auto& [_, row]: m_map) {
			table.append(row);
		}

		return table;
	}

	Index&& MemTable::createIndex(const size_t fileId) const {
		std::shared_lock lock(m_mutex);

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

	void MemTable::setCompression(ICompressionPtr compressor) {
		std::unique_lock lock(m_mutex);
		m_compressor = std::move(compressor);
	}

	MemTable::MemTable() {
		m_compressor = createCompressor(CompressionType::NoCompression);
		m_hasher = createHasher(HashType::NoHash);
	}

	void MemTable::setHasher(IHasherPtr hasher) {
		std::unique_lock lock(m_mutex);
		m_hasher = std::move(hasher);
	}
}
