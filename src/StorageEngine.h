#pragma once

#include "MemTable.h"

#include <atomic>
#include <filesystem>

namespace omx {

	class StorageEngine {
	public:
		StorageEngine() = default;

		explicit StorageEngine(std::string dir);

		void open(std::string dir);

		void put(Key key, const std::string& value);

		void remove(Key key);

		bool get(Key key, std::string& value) const;

		void load();

	private:
		std::unique_ptr<MemTable> m_memTable;
		Index m_index;

		std::atomic<size_t> m_segmentId = 0;
		const size_t m_memTableLimit = 1 * 1024 * 1024; // 1 mb
		std::filesystem::path m_dir;
		std::string m_walFileName;
		std::string m_indexFileName;
	};

}
