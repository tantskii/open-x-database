#pragma once

#include "ICompression.h"

namespace omx {

	class NoCompression : public ICompression {

		void compress(const std::string& input, std::string& output) override;

		void uncompress(const std::string& input, std::string& output) override;

	};

}
