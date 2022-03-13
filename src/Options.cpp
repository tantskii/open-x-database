#include <omx/Options.h>

#include "OptionsImpl.h"

namespace omx {

	namespace db = leveldb;

	void Options::setMaxFileSize(size_t value) {
		m_impl->setMaxFileSize(value);
	}

	void Options::setWriteBufferSize(size_t value) {
		m_impl->setWriteBufferSize(value);
	}

	void Options::setBlockSize(size_t value) {
		m_impl->setBlockSize(value);
	}

	void Options::setMaxOpenFiles(size_t value) {
		m_impl->setMaxOpenFiles(value);
	}

	void Options::setBlockCacheSize(size_t bytes) {
		m_impl->setBlockCacheSize(bytes);
	}

	size_t Options::getMaxFileSize() const {
		return m_impl->getMaxFileSize();
	}

	size_t Options::getWriteBufferSize() const {
		return m_impl->getWriteBufferSize();
	}

	size_t Options::getBlockSize() const {
		return m_impl->getBlockSize();
	}

	size_t Options::getMaxOpenFiles() const {
		return m_impl->getMaxOpenFiles();
	}

	size_t Options::getBlockCacheSize() const {
		return m_impl->getBlockCacheSize();
	}

	Options::~Options() = default;

	Options::Options() = default;

} // namespace omx