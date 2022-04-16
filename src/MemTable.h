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

namespace omx {

	class MemTable {
	public:

		MemTable();

		void put(Key key, const std::string& value);

		void remove(Key key);

		bool get(Key key, std::string& value);

		void dump(std::ostream& os);

		void setWriteAheadLog(const std::string& path);

		void setCompression(ICompressionPtr compressor);

		void setHasher(IHasherPtr hasher);

		void restoreFromLog(std::istream& stream);

		size_t getApproximateSize() const;

		SSTable createSortedStringsTable() const;

		Index createIndex(size_t fileId) const;

	private:

		void log(SSTableRowPtr row);

		std::map<InsertKey<Key>, SSTableRowPtr, std::less<>> m_map;
		size_t m_counter = 0;
		size_t m_memorySize = 0;

		bool m_isImmutable = false;

		WriteAheadLogPtr m_wal;
		ICompressionPtr m_compressor;
		IHasherPtr m_hasher;
	};
}


