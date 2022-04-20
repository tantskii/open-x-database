#pragma once

#include <omx/Options.h>

#include "MemTable.h"
#include "BloomFilter.h"

#include <atomic>
#include <filesystem>
#include <shared_mutex>
#include <mutex>

constexpr uint64_t kFilterSize = 10'000'000;
constexpr uint8_t kNumHashes = 7;

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
		std::filesystem::path m_indexDir;
		std::filesystem::path m_chunkDir;
		std::filesystem::path m_bloomFileName;
		std::string m_optionsFileName;
		Options m_opts;

		BloomFilter<kFilterSize, kNumHashes> m_bloomFilter;

		mutable std::shared_mutex m_mutex;
	};

}
