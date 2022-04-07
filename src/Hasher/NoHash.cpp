#include "NoHash.h"

namespace omx {
	std::pair<uint64_t, uint64_t> NoHash::hash(const std::string& data) {
		return {0, 0};
	}
}


