#pragma once

#include "internal/Export.h"

#include <cstdint>

namespace omx {

	struct OMXDB_EXPORT Key {
		Key() = default;

		explicit Key(uint64_t id_);

		bool operator==(const Key& other) const;

		uint64_t id = 0;
	};

}