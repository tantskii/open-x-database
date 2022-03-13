#pragma once

#include "Key.h"
#include "Bytes.h"
#include "Transaction.h"
#include "Options.h"

#include "internal/FastPimpl.h"
#include "internal/Export.h"

namespace omx {

	class BinaryStorageImpl;

	/**
	 * @brief A persistent ordered map from keys to values.
	 */
	class OMXDB_EXPORT BinaryStorage {
	public:
		BinaryStorage();

		~BinaryStorage();
		/**
		 * Open the database with the specified "name".
		 * @param path [in] path to database
		 * @param options [in] input options @see Options
		 */
		void open(const char* path, const Options& options = Options());

		/**
		 * @brief Set the database entry for "key" to "value".
		 * @param key [in] key @see Key
		 * @param value [in] value @see Bytes
		 * @throws std::runtime_error on error
		 */
		void put(Key key, const Bytes& value);

		/**
		 * @brief Remove the database entry (if any) for "key".
		 * @param key [in] key @see Key
		 * @throws std::runtime_error on error
		 */
		void remove(Key key);

		/**
		 * @brief If the database contains an entry for "key" store the corresponding value
		 * @param key [in] key @see Key
		 * @param value [out] value @see Bytes
		 * @throws std::runtime_error on error
		 */
		void get(Key key, Bytes& value);

		/**
		 * @brief Apply the specified updates to the database.
		 * @param transaction [in] transaction @see Transaction
		 * @throws std::runtime_error on error
		 */
		void execute(Transaction& transaction);

	private:
		static constexpr std::size_t kImplSize = 24;
		static constexpr std::size_t kImplAlign = 8;
		omx::FastPimpl<omx::BinaryStorageImpl, kImplSize, kImplAlign> m_impl;
	};

} // namespace omx