#include "MurmurHash3Adapter.h"

#include <MurmurHash3.h>

namespace omx {

	UInt128 MurmurHash3Adapter::hash(const std::string& data) {
		UInt128 hashValue = {};

		MurmurHash3_x64_128(data.data(), data.size(), 46, &hashValue);

		return hashValue;
	}
}
