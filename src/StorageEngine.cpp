#include "StorageEngine.h"
#include "Compression/ICompression.h"

#include <fstream>
#include <memory>
#include <utility>

namespace omx {

	namespace fs = std::filesystem;

	void StorageEngine::put(omx::Key key, const std::string& value) {
		std::string compressed;
		m_compressor->compress(value, compressed);

		auto checksum = m_hasher->hash(compressed);

		{
			auto lock = std::unique_lock(m_mutex);

			m_memTable->put(key, compressed, checksum);

			if (m_memTable->getApproximateSize() >= m_memTableLimit) {
				makeSnapshot();
			}
		}
	}

	void StorageEngine::remove(omx::Key key) {
		auto lock = std::unique_lock(m_mutex);
		m_memTable->remove(key);
	}

	bool StorageEngine::get(omx::Key key, std::string& value) const {
		omx::UInt128 checksum;
		std::string compressed;

		if (findInMemory(key, compressed, checksum) || findOnDisk(key, compressed, checksum)) {
			if (m_hasher->hash(compressed) != checksum) {
				throw std::runtime_error("data corrupted: invalid checksum");
			}

			m_compressor->uncompress(compressed, value);

			return true;
		}

		return false;
	}

	void StorageEngine::open(std::string dir, Options options) {
		m_opts = options;
		m_dir = std::move(dir);
		m_walFileName     = m_dir / "wal.bin";
		m_indexDir        = m_dir / "index";
		m_chunkDir        = m_dir / "segment";
		m_optionsFileName = m_dir / "options.bin";
		m_bloomFileName   = m_dir / "bloom_filter.bin";

		if (fs::exists(m_optionsFileName)) {
			loadOptions();
		} else {
			saveOptions();
		}

		m_compressor = createCompressor(m_opts.compressionType);
		m_hasher = createHasher(m_opts.hashType);

		m_memTableLimit = m_opts.maxMemTableSize;

		m_memTable = std::make_unique<MemTable>();
		m_index = std::make_unique<Index>();

		if (fs::exists(m_walFileName)) {
			auto stream = std::ifstream(m_walFileName, std::ios::binary | std::ios::in);
			m_memTable->restoreFromLog(stream);
		}
		m_memTable->setWriteAheadLog(m_walFileName);

		if (!fs::exists(m_chunkDir)) {
			fs::create_directory(m_chunkDir);
		}

		if (!fs::exists(m_indexDir)) {
			fs::create_directory(m_indexDir);
		}

		if (!fs::is_empty(m_indexDir)) {
			m_index->load(m_indexDir);
		}

		if (fs::exists(m_bloomFileName)) {
			auto stream = std::ifstream(m_bloomFileName, std::ios::binary | std::ios::in);
			m_bloomFilter.load(stream);
		}
	}

	void StorageEngine::saveOptions() const {
		auto stream = std::ofstream(m_optionsFileName, std::ios::binary);
		stream.write(reinterpret_cast<const char*>(&m_opts.maxMemTableSize), sizeof(m_opts.maxMemTableSize));
		stream.write(reinterpret_cast<const char*>(&m_opts.compressionType), sizeof(m_opts.compressionType));
		stream.flush();
	}

	void StorageEngine::loadOptions() {
		auto stream = std::ifstream(m_optionsFileName, std::ios::binary);
		stream.read(reinterpret_cast<char*>(&m_opts.maxMemTableSize), sizeof(m_opts.maxMemTableSize));
		stream.read(reinterpret_cast<char*>(&m_opts.compressionType), sizeof(m_opts.compressionType));
		stream.peek();
	}

	void StorageEngine::resetMemTable() {
		m_memTable = std::make_unique<MemTable>();
		m_memTable->setWriteAheadLog(m_walFileName);
	}

	void StorageEngine::makeSnapshot() {
		const auto segment       = m_segmentId++;
		const auto chunkFileName = m_chunkDir / ("segment_" + std::to_string(segment) + ".bin");
		const auto indexFileName = m_indexDir / ("index_" + std::to_string(segment) + ".bin");

		auto memTableStream = std::ofstream(chunkFileName, std::ios::binary);
		auto indexStream    = std::ofstream(indexFileName, std::ios::binary);
		auto bloomStream    = std::ofstream(m_bloomFileName, std::ios::binary);

		const auto table = m_memTable->createSortedStringsTable();
		for (const auto& row: table.getRowList()) {
			m_bloomFilter.add(row->getKey());
		}
		m_bloomFilter.dump(bloomStream);

		auto tableIndex = m_memTable->createSortedStringsTableIndex(segment);

		m_memTable->dump(memTableStream);

		tableIndex->dump(indexStream);

		m_index->update(std::move(tableIndex));

		resetMemTable();
	}

	bool StorageEngine::findInMemory(Key key, std::string& value, UInt128& checksum) const {
		auto lock = std::shared_lock(m_mutex);
		return m_memTable->get(key, value, checksum);
	}

	bool StorageEngine::findOnDisk(Key key, std::string& value, UInt128& checksum) const {
		if (!m_bloomFilter.probablyContains(key)) {
			return false;
		}

		SearchHint hint;
		{
			auto lock = std::shared_lock(m_mutex);
			if (!m_index->get(key, hint)) {
				return false;
			}
		}

		auto chunkName = "segment_" + std::to_string(hint.fileId) + ".bin";
		auto stream = std::ifstream(m_chunkDir / chunkName, std::ios::binary);

		std::string data;
		data.resize(hint.size);

		stream.seekg(hint.offset);
		stream.read(data.data(), data.size());
		stream.peek();

		auto row = deserialize(data);

		if (row->getOperationType() == EntryType::Remove) {
			return false;
		}

		value = row->getData();
		checksum = row->getChecksum();

		return true;
	}
}
