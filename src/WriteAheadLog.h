#pragma once

#include "Entry.h"

#include <ostream>
#include <memory>

namespace omx {

	class WriteAheadLog {
	public:
		explicit WriteAheadLog(std::ostream& stream);

		void log(const Entry& entry);

	private:
		std::ostream& m_stream;
	};

	using WriteAheadLogPtr = std::unique_ptr<WriteAheadLog>;
}
