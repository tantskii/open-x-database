#pragma once

#include <leveldb/db.h>
#include <leveldb/comparator.h>

namespace omx {

	class KeyComparator : public leveldb::Comparator {
	public:
		[[nodiscard]] int Compare(const leveldb::Slice& lhs, const leveldb::Slice& rhs) const override;

		[[nodiscard]] const char* Name() const override;

		void FindShortestSeparator(
			std::string* start,
			const leveldb::Slice& limit) const override;

		void FindShortSuccessor(std::string* start) const override;
	};

} // namespace omx
