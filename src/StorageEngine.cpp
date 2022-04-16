#include "StorageEngine.h"
#include "Compression/ICompression.h"

#include <fstream>
#include <memory>
#include <utility>

namespace omx {

	namespace fs = std::filesystem;

	void StorageEngine::put(omx::Key key, const std::string& value) {
		m_memTable->put(key, value);

		if (m_memTable->getApproximateSize() >= m_memTableLimit) {
			makeSnapshot();
		}
	}

	void StorageEngine::remove(omx::Key key) {
		m_memTable->remove(key);
	}

	bool StorageEngine::get(omx::Key key, std::string& value) const {
		if (findInMemory(key, value)) {
			return true;
		}

		return findOnDisk(key, value);
	}

	void StorageEngine::open(std::string dir) {
		m_dir = std::move(dir);
		m_walFileName     = m_dir / "wal.bin";
		m_indexFileName   = m_dir / "index.bin";
		m_optionsFileName = m_dir / "options.bin";

		if (fs::exists(m_optionsFileName)) {
			loadOptions();
		} else {
			saveOptions();
		}

		m_compressor = createCompressor(m_opts.compressionType);
		m_hasher = createHasher(m_opts.hashType);

		m_memTableLimit = m_opts.maxMemTableSize;
		resetMemTable();

		if (fs::exists(m_walFileName)) {
			auto stream = std::ifstream(m_walFileName, std::ios::binary | std::ios::in);
			m_memTable->restoreFromLog(stream);
		}

		if (fs::exists(m_indexFileName)) {
			auto stream = std::ifstream(m_indexFileName, std::ios::binary | std::ios::in);
			m_index.load(stream);
		}
	}

	StorageEngine::StorageEngine(Options options) {
		m_opts = options;
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
		m_memTable->setCompression(m_compressor);
		m_memTable->setHasher(m_hasher);
	}

	void StorageEngine::makeSnapshot() {
		auto segment   = m_segmentId++;
		auto chunkName = "segment_" + std::to_string(segment) + ".bin";

		auto memTableStream = std::ofstream(m_dir / chunkName, std::ios::binary);
		auto indexStream    = std::ofstream(m_indexFileName, std::ios::binary | std::ios::app);

		auto index = m_memTable->createIndex(segment);
		m_memTable->dump(memTableStream);

		index.dump(indexStream);
		m_index.merge(index);

		resetMemTable();
	}

	bool StorageEngine::findInMemory(Key key, std::string& value) const {
		return m_memTable->get(key, value);
	}

	bool StorageEngine::findOnDisk(Key key, std::string& value) const {
		SearchHint hint;
		if (!m_index.get(key, hint)) {
			return false;
		}

		auto chunkName = "segment_" + std::to_string(hint.fileId) + ".bin";
		auto stream = std::ifstream(m_dir / chunkName, std::ios::binary);

		std::string data;
		data.resize(hint.size);

		stream.seekg(hint.offset);
		stream.read(data.data(), data.size());
		stream.peek();

		auto row = deserialize(data);

		if (row->getOperationType() == EntryType::Remove) {
			return false;
		}

		const auto& compressed = row->getData();
		const auto checksum = m_hasher->hash(compressed);

		if (row->getChecksum() != checksum) {
			throw std::runtime_error("data corrupted");
		}

		m_compressor->uncompress(compressed, value);

		return true;
	}

	StorageEngine::~StorageEngine() {
		makeSnapshot();
	}
}
