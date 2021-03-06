#pragma once

#include <cstdint>

namespace omx {
	enum class CompressionType : uint8_t {
		NoCompression = 0,
		Snappy,
		LZ4
	};

	enum class HashType : uint8_t {
		NoHash = 0,
		CityHash128,
		MurmurHash3
	};

	struct Options {

		uint64_t maxMemTableSize = 1 * 1024 * 1024; // 1mb

		uint32_t maxWalBufferSize = 1 * 1024; // 1kb

		CompressionType compressionType = CompressionType::LZ4;

		HashType hashType = HashType::CityHash128;

	};
}


