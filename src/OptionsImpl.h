#pragma once

#include <leveldb/options.h>
#include <leveldb/cache.h>

namespace omx {

	class OptionsImpl {
	public:
		void setMaxFileSize(size_t bytes);

		void setWriteBufferSize(size_t bytes);

		void setBlockSize(size_t bytes);

		void setMaxOpenFiles(size_t count);

		void setBlockCacheSize(size_t bytes);

		[[nodiscard]] size_t getMaxFileSize() const;

		[[nodiscard]] size_t getWriteBufferSize() const;

		[[nodiscard]] size_t getBlockSize() const;

		[[nodiscard]] size_t getMaxOpenFiles() const;

		[[nodiscard]] size_t getBlockCacheSize() const;

	private:
		size_t m_blockCacheSize = 8 * 1024 * 1024;
		leveldb::Options m_opts;
	};

} // namespace omx
