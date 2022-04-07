#pragma once

#include "IHasher.h"

namespace omx {

	class NoHash : public IHasher {

		std::pair<uint64_t, uint64_t> hash(const std::string& data) override;

	};

}
