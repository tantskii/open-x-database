#include "NoCompression.h"

namespace omx {

	void NoCompression::compress(const std::string& input, std::string& output) {
		output = input;
	}

	void NoCompression::uncompress(const std::string& input, std::string& output) {
		output = input;
	}
}
