#pragma once

#include <omx/Options.h>

#include <memory>
#include <string>

namespace omx {

	using UInt128 = std::pair<uint64_t, uint64_t>;

	inline bool operator==(const UInt128& lhs, const UInt128& rhs) {
		return lhs.first == rhs.first && lhs.second == rhs.second;
	}

	class IHasher {
	public:
		virtual UInt128 hash(const std::string& data) = 0;
	};

	using IHasherPtr = std::shared_ptr<IHasher>;

	IHasherPtr createHasher(HashType hashType);

	const char* toString(HashType hashType);
}
