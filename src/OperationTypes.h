#pragma once

#include <cstdint>

namespace omx {
	enum class Operation : uint8_t {
		Read = 0,
		Put = 1,
		Remove = 2,
	};
}
