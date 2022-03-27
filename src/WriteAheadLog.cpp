#include "WriteAheadLog.h"

namespace omx {

	WriteAheadLog::WriteAheadLog(std::ostream& stream)
		: m_stream(stream)
	{}

	void WriteAheadLog::log(const Entry& entry) {
		entry.serialize(m_stream);
	}


}
