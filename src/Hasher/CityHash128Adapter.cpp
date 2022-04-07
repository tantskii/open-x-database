#include "CityHash128Adapter.h"

#include "CityHash/City.h"


std::pair<uint64_t, uint64_t> omx::CityHash128Adapter::hash(const std::string& data) {
	return CityHash128(data.c_str(), data.size());
}
