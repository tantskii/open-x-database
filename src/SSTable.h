#pragma once

#include "SSTableRow.h"

#include <vector>

namespace omx {

	class SSTable {
	public:

	private:
		std::vector<SSTableRow> m_rows;
	};
}

