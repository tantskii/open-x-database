#include <omx/BinaryStorage.h>

#include "BinaryStorageImpl.h"

namespace omx {

	namespace db = leveldb;

	void BinaryStorage::put(Key key, const Bytes& value) {
		m_impl->put(key, value);
	}

	void BinaryStorage::remove(Key key) {
		m_impl->remove(key);
	}

	void BinaryStorage::get(Key key, Bytes& value) {
		m_impl->get(key, value);
	}

	void BinaryStorage::open(const char* path, const Options& options) {
		m_impl->open(path, options);
	}

	void BinaryStorage::execute(Transaction& transaction) {
		m_impl->execute(transaction);
	}

	BinaryStorage::BinaryStorage() = default;

	BinaryStorage::~BinaryStorage() = default;

} // namespace omx