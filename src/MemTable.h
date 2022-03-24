#pragma once

#include <omx/Bytes.h>
#include <omx/Key.h>

#include "KeyComparator.h"

#include <map>
#include <shared_mutex>
#include <mutex>

namespace omx {

	class MemTable {
	public:

		struct Comparator {
			bool operator()(const Key& lhs, const Key& rhs) const;
		};

		void put(Key key, const Bytes& value);

		void remove(Key key);

		void get(Key key, Bytes& value);

//		TODO
//		void execute(Transaction& transaction);

	private:
		std::map<Key, Bytes, Comparator> m_map;
		mutable std::shared_mutex m_mutex;
	};
}


