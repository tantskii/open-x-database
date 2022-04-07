#pragma once

#include <cstdint>

namespace omx {
	enum class CompressionType : uint8_t {
		NoCompression = 0,
		Snappy
	};

	enum class HashType : uint8_t {
		NoHash = 0,
		CityHash128
	};

	struct Options {

		uint64_t maxMemTableSize = 1 * 1024 * 1024; // 1mb

		CompressionType compressionType = CompressionType::Snappy;

		HashType hashType = HashType::CityHash128;

	};
}


