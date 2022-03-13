#include "KeyComparator.h"

#include <omx/Key.h>

namespace omx {
	namespace db = leveldb;

	int KeyComparator::Compare(const db::Slice& lhs, const db::Slice& rhs) const {
		const auto* lhsKey = reinterpret_cast<const Key*>(lhs.data());
		const auto* rhsKey = reinterpret_cast<const Key*>(rhs.data());
		if (lhsKey->id == rhsKey->id) {
			return 0;
		}

		return lhsKey->id < rhsKey->id ? -1 : 1;
	}

	const char* KeyComparator::Name() const {
		return "KeyComparator";
	}

	void KeyComparator::FindShortestSeparator(std::string*, const db::Slice&) const {
		/*pass*/
	}

	void KeyComparator::FindShortSuccessor(std::string*) const {
		/*pass*/
	}
}
