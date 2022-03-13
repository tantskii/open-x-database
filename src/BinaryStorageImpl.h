#pragma once

#include "KeyComparator.h"

#include <omx/Key.h>
#include <omx/Options.h>
#include <omx/Transaction.h>

#include <leveldb/db.h>
#include <leveldb/cache.h>

#include <memory>

namespace omx {

	class BinaryStorageImpl {
	public:
		void open(const char* path, const Options& options = Options());

		void put(Key key, const Bytes& value);

		void remove(Key key);

		void get(Key key, Bytes& value);

		void execute(Transaction& transaction);

	private:
		std::unique_ptr<leveldb::DB> m_database;
		std::unique_ptr<leveldb::Cache> m_cache;

		KeyComparator m_keyComparator;
	};

} // namespace omx