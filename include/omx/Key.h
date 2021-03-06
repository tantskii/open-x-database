#pragma once

#include "internal/Export.h"

#include <cstdint>

namespace omx {

	struct OMXDB_EXPORT Key {
		Key() = default;

		explicit Key(uint64_t id_);

		bool operator==(const Key& other) const;
		bool operator<(const Key& other) const;

		uint64_t id = 0;
	};

	struct OMXDB_EXPORT KeyHasher {
		std::size_t operator()(const Key& key) const;
	};

}