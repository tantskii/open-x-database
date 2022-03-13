#pragma once

#include "Key.h"
#include "Bytes.h"
#include "internal/Export.h"
#include "internal/FastPimpl.h"

namespace omx {
	class TransactionImpl;

	/**
	 * @brief Transaction holds a collection of updates to apply atomically to a DB.
	 * The updates are applied in the order in which they are added to the Transaction.
	 */
	class OMXDB_EXPORT Transaction {
	public:
		Transaction();

		~Transaction();

		/**
		 * @brief Store the mapping "key->value" in the database.
		 * @param key [in] key @see Key
		 * @param value [in] value @see Bytes
		 */
		void put(Key key, const Bytes& value);

		/**
		 * @brief If the database contains a mapping for "key", erase it.  Else do nothing.
		 * @param key [in] key @see Key
		 */
		void remove(Key key);

		/**
		 * @brief Clear all updates buffered in this batch.
		 */
		void clear();

		TransactionImpl* get();

	private:
		static constexpr std::size_t kImplSize = 32;
		static constexpr std::size_t kImplAlign = 8;
		omx::FastPimpl<TransactionImpl, kImplSize, kImplAlign> m_impl;
	};

} // namespace omx
