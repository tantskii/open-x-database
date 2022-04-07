#include "IHasher.h"
#include "NoHash.h"
#include "CityHash128Adapter.h"

namespace omx {

	IHasherPtr createHasher(HashType hashType) {
		switch (hashType) {
			case HashType::NoHash:
				return std::make_shared<NoHash>();
			case HashType::CityHash128:
				return std::make_shared<CityHash128Adapter>();
			default:
				return nullptr;
		}
	}

}
