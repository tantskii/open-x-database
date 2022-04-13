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
			size_t segment = m_segmentId++;
			std::string chunkName = "segment_" + std::to_string(segment) + ".bin";
			std::ofstream stream(m_dir / chunkName, std::ios::binary);
			std::ofstream indexStream(m_indexFileName, std::ios::binary | std::ios::app);

			Index index;
			m_memTable->dump(segment, stream, index);
			index.dump(indexStream);
			m_index.merge(index);

			m_memTable = std::make_unique<MemTable>();
			m_memTable->setWriteAheadLog(m_walFileName);
			m_memTable->setCompression(m_compressor);
			m_memTable->setHasher(m_hasher);
		}
	}

	void StorageEngine::remove(omx::Key key) {
		m_memTable->remove(key);
	}

	bool StorageEngine::get(omx::Key key, std::string& value) const {
		if (m_memTable->get(key, value))
			return true;

		SearchHint hint;
		if (!m_index.get(key, hint)) {
			return false;
		}

		std::string chunkName = "segment_" + std::to_string(hint.fileId) + ".bin";
		std::ifstream stream(m_dir / chunkName, std::ios::binary);

		std::string data;
		data.resize(hint.size);

		stream.seekg(hint.offset);
		stream.read(data.data(), data.size());
		stream.peek();

		SSTableRowPtr row = deserialize(data);

		if (row->getOperationType() == EntryType::Remove) {
			return false;
		}

		const std::string& compressed = row->getData();
		UInt128 checksum = m_hasher->hash(compressed);
		if (row->getChecksum() != checksum) {
			throw std::runtime_error("data corrupted");
		}

		m_compressor->uncompress(compressed, value);

		return true;
	}

	void StorageEngine::load() {
		loadOptions();

		m_compressor = createCompressor(m_opts.compressionType);
		m_hasher = createHasher(m_opts.hashType);
		m_memTableLimit = m_opts.maxMemTableSize;

		std::ifstream memTableStream(m_walFileName, std::ios::binary | std::ios::in);
		m_memTable->restoreFromLog(memTableStream);
		m_memTable->setWriteAheadLog(m_walFileName);
		m_memTable->setCompression(m_compressor);
		m_memTable->setHasher(m_hasher);

		std::ifstream indexStream(m_indexFileName, std::ios::binary | std::ios::in);
		m_index.load(indexStream);
	}

	void StorageEngine::open(std::string dir) {
		m_dir = std::move(dir);
		m_walFileName     = m_dir / "wal.bin";
		m_indexFileName   = m_dir / "index.bin";
		m_optionsFileName = m_dir / "options.bin";

		m_compressor = createCompressor(m_opts.compressionType);
		m_hasher = createHasher(m_opts.hashType);

		m_memTableLimit = m_opts.maxMemTableSize;
		m_memTable = std::make_unique<MemTable>();
		m_memTable->setCompression(m_compressor);
		m_memTable->setHasher(m_hasher);

		if (fs::exists(m_walFileName) && fs::exists(m_indexFileName)) {
			load();
		} else {
			m_memTable->setWriteAheadLog(m_walFileName);
		}

		saveOptions();
	}

	StorageEngine::StorageEngine(std::string dir, Options options) {
		m_opts = options;

		open(std::move(dir));
	}

	void StorageEngine::saveOptions() const {
		std::ofstream stream(m_optionsFileName, std::ios::binary);
		stream.write(reinterpret_cast<const char*>(&m_opts.maxMemTableSize), sizeof(m_opts.maxMemTableSize));
		stream.write(reinterpret_cast<const char*>(&m_opts.compressionType), sizeof(m_opts.compressionType));
		stream.flush();
	}

	void StorageEngine::loadOptions() {
		std::ifstream stream(m_optionsFileName, std::ios::binary);
		stream.read(reinterpret_cast<char*>(&m_opts.maxMemTableSize), sizeof(m_opts.maxMemTableSize));
		stream.read(reinterpret_cast<char*>(&m_opts.compressionType), sizeof(m_opts.compressionType));
		stream.peek();
	}
}
