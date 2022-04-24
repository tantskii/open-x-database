#include <omx/Database.h>

#include "StorageEngine.h"

namespace omx {

	void Database::put(Key key, const std::string& value) {
		m_impl->put(key, value);
	}

	void Database::remove(Key key) {
		m_impl->remove(key);
	}

	bool Database::get(Key key, std::string& value) {
		return m_impl->get(key, value);
	}

	void Database::open(const char* path, Options options) {
		m_impl->open(path, options);
	}

	Response Database::handle(const Request& request) noexcept {
		return m_impl->handle(request);
	}

	Database::Database() = default;

	Database::~Database() = default;

} // namespace omx