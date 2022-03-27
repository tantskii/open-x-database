#pragma once

#include <omx/Bytes.h>
#include <omx/Key.h>

#include "KeyComparator.h"
#include "Entry.h"
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

		struct Comparator {
			bool operator()(const Key& lhs, const Key& rhs) const;
		};

		void put(Key key, const std::string& value);

		void remove(Key key);

		bool get(Key key, std::string& value);

//		TODO
//		void execute(Transaction& transaction);

		void dump(size_t fileId, std::ostream& os, Index& index);

		void setWriteAheadLog(std::ostream& stream);

		void restoreFromLog(std::istream& stream);

	private:

		void log(const Entry& entry);

		std::map<InsertKey<Key, Comparator>, Entry, std::less<>> m_map;
		mutable std::shared_mutex m_mutex;
		size_t m_counter = 0;

		bool m_isClosed = false;

		WriteAheadLogPtr m_wal;
	};
}


