#pragma once

#include "MemTable.h"

#include <atomic>
#include <filesystem>

namespace omx {

	class StorageEngine {
	public:
		explicit StorageEngine(std::string dir);

		void put(Key key, const std::string& value);

		void remove(Key key);

		bool get(Key key, std::string& value) const;

		void load();

	private:
		std::unique_ptr<MemTable> m_memTable;
		Index m_index;

		std::atomic<size_t> m_chunkId = 0;
		const size_t m_memTableLimit = 1 * 1024 * 1024; // 1 mb
		const std::filesystem::path m_dir;
		const std::string m_walFileName;
		const std::string m_indexFileName;
	};

}
