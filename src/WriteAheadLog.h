#pragma once

#include "SSTableRow.h"

#include <ostream>
#include <memory>

namespace omx {

	class WriteAheadLog {
	public:
		explicit WriteAheadLog(std::ostream& stream);

		void log(const SSTableRow& row);

	private:
		std::ostream& m_stream;
	};

	using WriteAheadLogPtr = std::unique_ptr<WriteAheadLog>;
}
