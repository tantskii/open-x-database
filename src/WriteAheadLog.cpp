#include "WriteAheadLog.h"

namespace omx {

	WriteAheadLog::WriteAheadLog(const std::string& path, uint32_t bufferSize)
		: m_stream(path, std::ios::binary | std::ios::out)
		, m_buffer(bufferSize)
	{
		m_stream.rdbuf()->pubsetbuf(m_buffer.data(), m_buffer.size());
	}

	void WriteAheadLog::log(SSTableRowPtr row) {
		std::string data = serialize(row);
		m_stream.write(data.c_str(), data.size());
	}

	void WriteAheadLog::flushAndClear() {
		m_stream.flush();
		m_buffer.clear();
	}
}
