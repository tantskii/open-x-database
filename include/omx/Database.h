#pragma once

#include "Key.h"
#include "Options.h"
#include "Request.h"

#include "internal/FastPimpl.h"
#include "internal/Export.h"

#include <string>

namespace omx {

	class StorageEngine;

	/**
	 * @brief A persistent ordered map from keys to values.
	 */
	class OMXDB_EXPORT Database {
	public:
		Database();

		~Database();

		/**
		 * Open the database with the specified "name".
		 * @param path [in] path to database
		 * @param options [in] input options @see Options
		 */
		void open(const char* path, Options options = {});

		/**
		 * @brief Set the database entry for "key" to "value".
		 * @param key [in] key @see Key
		 * @param value [in] value @see Bytes
		 * @throws std::runtime_error on error
		 */
		void put(Key key, const std::string& value);

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
		bool get(Key key, std::string& value);

		/**
		 * @brief get, put and remove functions in a single method
		 * @param request [in] object that describes request
		 * @return database response
		 */
		Response handle(const Request& request) noexcept;

	private:
		static constexpr std::size_t kImplSize = 384;
		static constexpr std::size_t kImplAlign = 8;
		omx::FastPimpl<omx::StorageEngine, kImplSize, kImplAlign> m_impl;
	};

} // namespace omx