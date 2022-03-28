#include "SSTable.h"

namespace omx {

	template <typename InputIt, typename OutputIt>
	void copyFirstKeyOccurrence(InputIt first, InputIt last, OutputIt dest) {
		Key prev = Key(std::string::npos);
		while (first != last) {
			Key curr = first->get()->getKey();
			if (curr != prev) {
				*dest = *first;
				++dest;
				prev = curr;
			}
			++first;
		}
	}

	template <typename InputIt>
	InputIt advanceToNextKey(InputIt first, InputIt last) {
		Key prev = first->get()->getKey();
		while (first < last && first->get()->getKey() == prev) {
			++first;
		}
		return first;
	}

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

		for (; first1 != last1; ++d_first) {
			if (first2 == last2) {
				copyFirstKeyOccurrence(first1, last1, d_first);
				return;
			}
			if (*first2 == *first1) {
				*d_first = *first1;
				first1 = advanceToNextKey(first1, last1);
				first2 = advanceToNextKey(first2, last2);
			} else if (*first2 < *first1) {
				*d_first = *first2;
				first2 = advanceToNextKey(first2, last2);
			} else if (*first2 > *first1) {
				*d_first = *first1;
				first1 = advanceToNextKey(first1, last1);
			}
		}
		copyFirstKeyOccurrence(first2, last2, d_first);

		result.shrink_to_fit();
		m_rows = result;
	}

	void SSTable::load(std::istream& stream) {
		while (stream.eof()) {
			auto row = std::make_shared<SSTableRow>();

			row->deserialize(stream);

			m_rows.push_back(std::move(row));
		}
	}

	void SSTable::dump(std::ostream& stream) {
		for (auto row: m_rows) {
			row->serialize(stream, false);
		}
		stream.flush();
	}

	const std::vector<SSTableRowPtr>& SSTable::getRowList() {
		return m_rows;
	}
}
