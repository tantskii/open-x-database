#include "WriteAheadLog.h"

namespace omx {

	WriteAheadLog::WriteAheadLog(const std::string& path)
		: m_stream(path, std::ios::out)
	{}

	void WriteAheadLog::log(const SSTableRow& row) {
		row.serialize(m_stream);
	}


}
