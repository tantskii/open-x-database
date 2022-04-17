#include <omx/Options.h>

#include "NoCompression.h"
#include "SnappyCompressionAdapter.h"

namespace omx {

	ICompressionPtr createCompressor(CompressionType compressionType) {
		switch (compressionType) {
			case CompressionType::NoCompression:
				return std::make_shared<NoCompression>();
			case CompressionType::Snappy:
				return std::make_shared<SnappyCompressionAdapter>();
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
			default:
				return "Unknown";
		}
	}

}



