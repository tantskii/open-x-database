#include "WriteAheadLog.h"

namespace omx {

	WriteAheadLog::WriteAheadLog(const std::string& path)
		: m_stream(path, std::ios::out)
	{}

	void WriteAheadLog::log(SSTableRowPtr row) {
		row->serialize(m_stream);
	}


}
