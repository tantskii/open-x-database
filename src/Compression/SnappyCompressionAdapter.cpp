#include "SnappyCompressionAdapter.h"

#include <snappy.h>

namespace omx {

	void SnappyCompressionAdapter::compress(const std::string& input, std::string& output) {
		snappy::Compress(input.c_str(), input.size(), &output);
	}

	void SnappyCompressionAdapter::uncompress(const std::string& input, std::string& output) {
		snappy::Uncompress(input.c_str(), input.size(), &output);
	}
}
