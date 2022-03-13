#include <omx/Transaction.h>

#include "TransactionImpl.h"

namespace omx {

	namespace db = leveldb;

	void Transaction::put(Key key, const Bytes& value) {
		m_impl->put(key, value);
	}

	void Transaction::remove(Key key) {
		m_impl->remove(key);
	}

	TransactionImpl* Transaction::get() {
		return m_impl.ptr();
	}

	void Transaction::clear() {
		m_impl->clear();
	}

	Transaction::~Transaction() = default;

	Transaction::Transaction() = default;

} // namespace omx
