#pragma once

#include "IHasher.h"

namespace omx {

	class CityHash128Adapter : public IHasher {

		std::pair<uint64_t, uint64_t> hash(const std::string& data) override;

	};

}
