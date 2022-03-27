#include "WriteAheadLog.h"

namespace omx {

	WriteAheadLog::WriteAheadLog(std::ostream& stream)
		: m_stream(stream)
	{}

	void WriteAheadLog::log(const SSTableRow& row) {
		row.serialize(m_stream);
	}


}
