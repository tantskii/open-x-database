#pragma once

#include <omx/Bytes.h>
#include <omx/Key.h>
#include <omx/Options.h>

#include <leveldb/write_batch.h>

namespace omx {

	class TransactionImpl : public leveldb::WriteBatch {
	public:
		void put(Key key, const Bytes& value);

		void remove(Key key);

		void clear();
	};

} // namespace omx
