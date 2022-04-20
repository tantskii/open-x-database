#pragma once

#include <omx/Key.h>

#include "SSTable.h"
#include "SSTableRow.h"
#include "MemTableKey.h"
#include "Index.h"
#include "WriteAheadLog.h"
#include "Compression/ICompression.h"
#include "Hasher/IHasher.h"

#include <map>
#include <shared_mutex>
#include <mutex>
#include <ostream>
#include <atomic>

namespace omx {

	class MemTable {
	public:

		MemTable();

		void put(Key key, const std::string& value);

		void put(Key key, const std::string& value, const UInt128& checksum);

		void remove(Key key);

		bool get(Key key, std::string& value);
		bool get(Key key, std::string& value, UInt128& checksum);

		void dump(std::ostream& os);

		void setWriteAheadLog(const std::string& path);

		void restoreFromLog(std::istream& stream);

		[[nodiscard]] size_t getApproximateSize() const;

		[[nodiscard]] SSTable createSortedStringsTable() const;

		[[nodiscard]] SSTableIndexPtr createSortedStringsTableIndex(size_t fileId) const;

		void makeImmutable();

	private:

		void put(Key key, const std::string& value, const UInt128& checksum, EntryType entryType);

		void log(SSTableRowPtr row);

		std::map<InsertKey<Key>, SSTableRowPtr, std::less<>> m_map;

		size_t m_counter = 0;
		std::atomic<size_t> m_memorySize = 0;

		std::atomic<bool> m_isImmutable = false;

		WriteAheadLogPtr m_wal;
	};

	using MemTablePtr = std::unique_ptr<MemTable>;
}


