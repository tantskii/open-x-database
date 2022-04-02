#include "WriteAheadLog.h"

namespace omx {

	WriteAheadLog::WriteAheadLog(const std::string& path)
		: m_stream(path, std::ios::out)
	{}

	void WriteAheadLog::log(SSTableRowPtr row) {
		std::string data = serialize(row);
		m_stream.write(data.c_str(), data.size());
		m_stream.flush();
	}

}
