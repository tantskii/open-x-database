#pragma once

#include "SSTableRow.h"

#include <vector>

namespace omx {

	class SSTable {
	public:

		void append(SSTableRowPtr row);

		void merge(const SSTable& other);

		void load(std::istream& stream);

		void dump(std::ostream& stream);

		const std::vector<SSTableRowPtr>& getRowList() const;

	private:
		std::vector<SSTableRowPtr> m_rows;
	};
}

