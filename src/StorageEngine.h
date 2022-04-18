#pragma once

#include <omx/Options.h>

#include "MemTable.h"

#include <atomic>
#include <filesystem>
#include <shared_mutex>
#include <mutex>

namespace omx {

	class StorageEngine {
	public:
		void open(std::string dir, Options options = {});

		void put(Key key, const std::string& value);

		void remove(Key key);

		bool get(Key key, std::string& value) const;

	private:

		void resetMemTable();

		void makeSnapshot();

		bool findInMemory(Key key, std::string& value, UInt128& checksum) const;
		bool findOnDisk(Key key, std::string& value, UInt128& checksum) const;

		void saveOptions() const;

		void loadOptions();

		MemTablePtr m_memTable;
		ICompressionPtr m_compressor;
		IHasherPtr m_hasher;
		IndexPtr m_index;

		std::atomic<size_t> m_segmentId = 0;
		size_t m_memTableLimit = 1 * 1024 * 1024; // 1 mb
		std::filesystem::path m_dir;
		std::string m_walFileName;
		std::string m_indexFileName;
		std::string m_optionsFileName;
		Options m_opts;

		mutable std::shared_mutex m_mutex;
	};

}
