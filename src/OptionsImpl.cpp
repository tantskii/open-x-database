#include "OptionsImpl.h"

namespace omx {

	namespace db = leveldb;

	void OptionsImpl::setMaxFileSize(size_t value) {
		m_opts.max_file_size = value;
	}

	void OptionsImpl::setWriteBufferSize(size_t value) {
		m_opts.write_buffer_size = value;
	}

	void OptionsImpl::setBlockSize(size_t value) {
		m_opts.block_size = value;
	}

	void OptionsImpl::setMaxOpenFiles(size_t value) {
		m_opts.max_open_files = value;
	}

	void OptionsImpl::setBlockCacheSize(size_t bytes) {
		m_blockCacheSize = bytes;
	}

	size_t OptionsImpl::getMaxFileSize() const {
		return m_opts.max_file_size;
	}

	size_t OptionsImpl::getWriteBufferSize() const {
		return m_opts.write_buffer_size;
	}

	size_t OptionsImpl::getBlockSize() const {
		return m_opts.block_size;
	}

	size_t OptionsImpl::getMaxOpenFiles() const {
		return m_opts.max_open_files;
	}

	size_t OptionsImpl::getBlockCacheSize() const {
		return m_blockCacheSize;
	}

} // namespace omx
