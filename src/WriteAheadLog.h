#pragma once

#include "SSTableRow.h"

#include <fstream>
#include <memory>

namespace omx {

	class WriteAheadLog {
	public:
		explicit WriteAheadLog(const std::string& path);

		void log(SSTableRowPtr row);

	private:
		std::ofstream m_stream;
	};

	using WriteAheadLogPtr = std::unique_ptr<WriteAheadLog>;
}
