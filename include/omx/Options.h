#pragma once

#include "internal/Export.h"
#include "internal/FastPimpl.h"

namespace omx {

	class OptionsImpl;

	/**
	 * @brief Options to control the behavior of a database
	 */
	class OMXDB_EXPORT Options {
	public:
		Options();
		~Options();

		/**
		 * @brief Sets amount of bytes that database will write up to file
		 * before switching to a new one.
		 * @param value [in] amount of bytes
		 */
		void setMaxFileSize(std::size_t bytes);

		/**
		 * @brief Sets amount of data in bytes to build up in memory
		 * before converting to a sorted on-disk file.
		 * @param value [in] amount of bytes
		 */
		void setWriteBufferSize(std::size_t bytes);

		/**
		 * Sets approximate size of user data in bytes packed per block.
		 * @param value [in] amount of bytes
		 */
		void setBlockSize(std::size_t bytes);

		/**
		 * Sets number of open files that can be used by the DB.
		 * @param value [in] number of open files
		 */
		void setMaxOpenFiles(std::size_t count);

		/**
		 * Sets cache size in bytes for blocks
		 * @param bytes [in] amount of bytes
		 */
		void setBlockCacheSize(std::size_t bytes);

		/**
		 * @brief Returns amount of bytes that database will write up to file
		 * before switching to a new one.
		 * @return amount of bytes
		 */
		[[nodiscard]] std::size_t getMaxFileSize() const;

		/**
		 * @brief Returns amount of data in bytes to build up in memory
		 * before converting to a sorted on-disk file.
		 * @return amount of bytes
		 */
		[[nodiscard]] std::size_t getWriteBufferSize() const;

		/**
		 * @brief Return approximate size of user data in bytes packed per block.
		 * @return amount of bytes
		 */
		[[nodiscard]] std::size_t getBlockSize() const;

		/**
		 * @brief Returns number of open files that can be used by the DB.
		 * @return number of open files
		 */
		[[nodiscard]] std::size_t getMaxOpenFiles() const;

		/**
		 * @brief Returns cache size in bytes for blocks
		 * @return amount of bytes
		 */
		[[nodiscard]] std::size_t getBlockCacheSize() const;

	private:
		static constexpr std::size_t kImplSize = 104;
		static constexpr std::size_t kImplAlign = 8;
		omx::FastPimpl<OptionsImpl, kImplSize, kImplAlign> m_impl;
	};

} // namespace omx
