#pragma once

#include <omx/Options.h>

#include <string>
#include <memory>

namespace omx {

	class ICompression {
	public:
		virtual void compress(const std::string& input, std::string& output) = 0;
		virtual void uncompress(const std::string& input, std::string& output) = 0;
	};

	using ICompressionPtr = std::shared_ptr<ICompression>;

	ICompressionPtr createCompressor(CompressionType compressionType);

	const char* toString(CompressionType compressionType);

}
