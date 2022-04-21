#include "SSTable.h"

#include <algorithm>
#include <iostream>

namespace omx {

	void SSTable::append(SSTableRowPtr row) {
		m_rows.push_back(row);
	}

	void SSTable::merge(const SSTable& other) {
		std::vector<SSTableRowPtr> result;
		result.reserve(m_rows.size() + other.m_rows.size());
		auto first1  = m_rows.cbegin();
		auto last1   = m_rows.cend();
		auto first2  = other.m_rows.cbegin();
		auto last2   = other.m_rows.cend();
		auto d_first = std::back_inserter(result);

		std::merge(first1, last1, first2, last2, d_first);

		auto comp = [](const SSTableRowPtr& lhs, const SSTableRowPtr& rhs) {
			return lhs->getKey() == rhs->getKey();
		};

		auto last = std::unique(result.begin(), result.end(), comp);

		result.erase(last, result.end());
		result.shrink_to_fit();

		m_rows = std::move(result);
	}

	void SSTable::load(std::istream& stream) {
		while (!stream.eof()) {
			auto row = deserialize(stream);

			m_rows.push_back(std::move(row));

			stream.peek();
		}
	}

	void SSTable::dump(std::ostream& stream) {
		std::string data;
		for (auto row: m_rows) {
			data = serialize(row);
			stream.write(data.c_str(), data.size());
		}
		stream.flush();
	}

	const std::vector<SSTableRowPtr>& SSTable::getRowList() const {
		return m_rows;
	}
}
