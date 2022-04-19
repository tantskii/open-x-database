#pragma once

#include "ICompression.h"

namespace omx {

	class LZ4CompressionAdapter : public ICompression {

		void compress(const std::string& input, std::string& output) override;

		void uncompress(const std::string& input, std::string& output) override;
	};

}
