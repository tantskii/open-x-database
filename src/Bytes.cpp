#include <omx/Bytes.h>

namespace omx {

	size_t Bytes::bytesSize() const {
		return m_data.size();
	}

	std::string& Bytes::toString() {
		return m_data;
	}

	const std::string& Bytes::toString() const {
		return m_data;
	}

	const char* Bytes::toCString() const {
		return m_data.c_str();
	}

	void Bytes::clear() {
		m_data.clear();
	}

	bool Bytes::empty() const {
		return m_data.empty();
	}

} // namespace omx
