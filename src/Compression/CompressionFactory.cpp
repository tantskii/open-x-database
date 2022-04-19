#include <omx/Options.h>

#include "NoCompression.h"
#include "SnappyCompressionAdapter.h"
#include "LZ4CompressionAdapter.h"

namespace omx {

	ICompressionPtr createCompressor(CompressionType compressionType) {
		switch (compressionType) {
			case CompressionType::NoCompression:
				return std::make_shared<NoCompression>();
			case CompressionType::Snappy:
				return std::make_shared<SnappyCompressionAdapter>();
			case CompressionType::LZ4:
				return std::make_shared<LZ4CompressionAdapter>();
			default:
				return std::make_shared<NoCompression>();
		}
	}

	const char* toString(CompressionType compressionType) {
		switch (compressionType) {
			case CompressionType::NoCompression:
				return "NoCompression";
			case CompressionType::Snappy:
				return "Snappy";
			case CompressionType::LZ4:
				return "LZ$";
			default:
				return "Unknown";
		}
	}

}



