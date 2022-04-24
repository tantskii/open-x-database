#include "IHasher.h"
#include "NoHash.h"
#include "CityHash128Adapter.h"
#include "MurmurHash3Adapter.h"

namespace omx {

	IHasherPtr createHasher(HashType hashType) {
		switch (hashType) {
			case HashType::NoHash:
				return std::make_shared<NoHash>();
			case HashType::CityHash128:
				return std::make_shared<CityHash128Adapter>();
			case HashType::MurmurHash3:
				return std::make_shared<MurmurHash3Adapter>();
			default:
				return nullptr;
		}
	}

	const char* toString(HashType hashType) {
		switch (hashType) {
			case HashType::NoHash:
				return "NoHash";
			case HashType::CityHash128:
				return "CityHash128";
			case HashType::MurmurHash3:
				return "MurmurHash3";
			default:
				return "Unknown";
		}
	}

}

