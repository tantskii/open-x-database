#pragma once

#include "SSTableRow.h"

#include <fstream>
#include <memory>
#include <vector>

namespace omx {

	class WriteAheadLog {
	public:
		explicit WriteAheadLog(const std::string& path, uint32_t bufferSize = 512);

		void log(SSTableRowPtr row);

		void flushAndClear();

	private:
		std::ofstream m_stream;
		std::vector<char> m_buffer;
	};

	using WriteAheadLogPtr = std::unique_ptr<WriteAheadLog>;
}
