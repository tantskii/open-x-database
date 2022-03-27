#include "StorageEngine.h"

#include <fstream>
#include <memory>

namespace omx {

	namespace fs = std::filesystem;

	void StorageEngine::put(omx::Key key, const std::string& value) {
		m_memTable->put(key, value);

		if (m_memTable->getApproximateSize() >= m_memTableLimit) {
			size_t chunkId = m_chunkId++;
			std::string chunkName = "chunk_" + std::to_string(chunkId) + ".bin";
			std::ofstream stream(m_dir / chunkName, std::ios::binary);
			std::ofstream indexStream(m_indexFileName, std::ios::binary | std::ios::app);

			Index index;
			m_memTable->dump(chunkId, stream, index);
			index.dump(indexStream);
			m_index.merge(index);

			m_memTable = std::make_unique<MemTable>();
			m_memTable->setWriteAheadLog(m_walFileName);
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

		std::string chunkName = "chunk_" + std::to_string(hint.fileId) + ".bin";
		std::ifstream stream(m_dir / chunkName, std::ios::binary);
		SSTableRow row;

		stream.seekg(hint.offset);
		row.deserialize(stream);

		if (row.getOperationType() == EntryType::Remove) {
			return false;
		}

		value = row.getData();

		return true;
	}

	void StorageEngine::load() {
		std::ifstream memTableStream(m_walFileName, std::ios::binary | std::ios::in);
		m_memTable->restoreFromLog(memTableStream);
		m_memTable->setWriteAheadLog(m_walFileName);

		std::ifstream indexStream(m_indexFileName, std::ios::binary | std::ios::in);
		m_index.load(indexStream);
	}

	StorageEngine::StorageEngine(std::string dir)
		: m_memTable(new MemTable())
		, m_dir(std::move(dir))
		, m_walFileName(m_dir / "wal.bin")
		, m_indexFileName(m_dir / "index.bin")
	{
		if (fs::is_regular_file(m_indexFileName) && fs::is_regular_file(m_indexFileName)) {
			load();
		} else {
			m_memTable->setWriteAheadLog(m_walFileName);
		}
	}

}
