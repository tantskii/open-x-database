#include "LZ4CompressionAdapter.h"

#include <lz4.h>

#include <stdexcept>

namespace omx {

	void LZ4CompressionAdapter::compress(const std::string& input, std::string& output) {
		constexpr uint8_t sizeOfInputSize = sizeof(uint64_t);
		auto inputSize = static_cast<uint64_t>(input.size());

		const int maxCompressedSize = LZ4_compressBound(input.size());

		if (maxCompressedSize <= 0) {
			throw std::runtime_error("Invalid input size for LZ4 compression: "
				+ std::to_string(input.size()));
		}

		output.resize(maxCompressedSize + sizeOfInputSize);

		const int compressedSize = LZ4_compress_default(
			input.c_str(),
			output.data() + sizeOfInputSize,
			input.size(),
			output.size() - sizeOfInputSize);

		if (compressedSize <= 0) {
			throw std::runtime_error("result " + std::to_string(compressedSize)
				+ " indicates a failure trying to compress the data.");
		}

		std::copy(
			reinterpret_cast<const char*>(&inputSize),
			reinterpret_cast<const char*>(&inputSize) + sizeOfInputSize,
			output.data()
		);

		output.resize(compressedSize + sizeOfInputSize);
	}

	void LZ4CompressionAdapter::uncompress(const std::string& input, std::string& output) {
		constexpr uint8_t sizeOfOutputSize = sizeof(uint64_t);

		const uint64_t outputSize = *(reinterpret_cast<const uint64_t*>(input.c_str()));

		output.resize(outputSize);

		const int decompressedSize = LZ4_decompress_safe(
			input.c_str() + sizeOfOutputSize,
			output.data(),
			input.size() - sizeOfOutputSize,
			output.size());

		if (decompressedSize <= 0) {
			throw std::runtime_error("result " + std::to_string(decompressedSize)
				+ " indicates a failure trying to uncompress the data." );
		}

		output.resize(decompressedSize);
	}
}