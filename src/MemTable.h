#pragma once

#include <omx/Bytes.h>
#include <omx/Key.h>

#include "KeyComparator.h"
#include "SSTable.h"
#include "SSTableRow.h"
#include "MemTableKey.h"
#include "Index.h"
#include "WriteAheadLog.h"

#include <map>
#include <shared_mutex>
#include <mutex>
#include <ostream>

namespace omx {

	class MemTable {
	public:

		void put(Key key, const std::string& value);

		void remove(Key key);

		bool get(Key key, std::string& value);

//		TODO
//		void execute(Transaction& transaction);

		void dump(size_t fileId, std::ostream& os, Index& index);

		void setWriteAheadLog(const std::string& path);

		void restoreFromLog(std::istream& stream);

		size_t getApproximateSize() const;

		SSTable createSortedStringsTable() const;

		Index&& createIndex(size_t fileId) const;

	private:

		void log(SSTableRowPtr row);

		std::map<InsertKey<Key>, SSTableRowPtr, std::less<>> m_map;
		mutable std::shared_mutex m_mutex;
		size_t m_counter = 0;
		size_t m_memorySize = 0;

		bool m_isImmutable = false;

		WriteAheadLogPtr m_wal;
	};
}


