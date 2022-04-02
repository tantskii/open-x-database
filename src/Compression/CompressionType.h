#pragma once

#include <cstdint>

namespace omx {
	enum class CompressionType : uint8_t {
		NoCompression = 0,
		Snappy
	};
}
