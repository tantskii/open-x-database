#include "TransactionImpl.h"

namespace omx {

	namespace db = leveldb;

	void TransactionImpl::put(Key key, const Bytes& value) {
		db::Slice keySlice = db::Slice(
			reinterpret_cast<const char*>(&key),
			sizeof(key));

		db::Slice valSlice = db::Slice(value.toCString(), value.bytesSize());

		Put(keySlice, valSlice);
	}

	void TransactionImpl::remove(Key key) {
		db::Slice keySlice = db::Slice(
			reinterpret_cast<const char*>(&key),
			sizeof(key));

		Delete(keySlice);
	}

	void TransactionImpl::clear() {
		Clear();
	}

} // namespace omx
