#include "BinaryStorageImpl.h"

namespace omx {
	namespace db = leveldb;

	void BinaryStorageImpl::put(Key key, const Bytes& value) {
		const db::Slice keySlice = db::Slice(
			reinterpret_cast<const char*>(&key),
			sizeof(key));

		const db::Slice valSlice = db::Slice(value.toCString(), value.bytesSize());

		const db::WriteOptions options;

		const db::Status status = m_database->Put(options, keySlice, valSlice);

		if (!status.ok()) {
			throw std::runtime_error(status.ToString());
		}
	}

	void BinaryStorageImpl::remove(Key key) {
		const db::Slice keySlice = db::Slice(
			reinterpret_cast<const char*>(&key),
			sizeof(key));
		const db::WriteOptions options;

		const db::Status status = m_database->Delete(options, keySlice);

		if (!status.ok()) {
			throw std::runtime_error(status.ToString());
		}
	}

	void BinaryStorageImpl::get(Key key, Bytes& value) {
		const db::Slice keySlice = db::Slice(
			reinterpret_cast<const char*>(&key),
			sizeof(key));

		db::ReadOptions options;
		options.verify_checksums = true;

		const db::Status status = m_database->Get(options, keySlice, &value.toString());

		if (!status.ok()) {
			throw std::runtime_error(status.ToString());
		}
	}

	void BinaryStorageImpl::open(const char* path, const Options& options) {
		db::Options opts;
		opts.max_open_files = options.getMaxOpenFiles();
		opts.write_buffer_size = options.getWriteBufferSize();
		opts.max_file_size = options.getMaxFileSize();
		opts.block_size = options.getBlockSize();
		opts.block_cache = db::NewLRUCache(options.getBlockCacheSize());
		opts.comparator = &m_keyComparator;
		opts.create_if_missing = true;

		db::DB* database;
		const db::Status status = db::DB::Open(opts, path, &database);
		if (!status.ok()) {
			throw std::runtime_error(status.ToString());
		}

		m_database.reset(database);
	}

	void BinaryStorageImpl::execute(Transaction& transaction) {
		const db::WriteOptions options;

		const db::Status status = m_database->Write(options,
			reinterpret_cast<leveldb::WriteBatch*>(transaction.get()));
		if (!status.ok()) {
			throw std::runtime_error(status.ToString());
		}
	}

} // namespace omx
