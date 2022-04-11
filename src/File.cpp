#include "File.h"

#include <fstream>

namespace fs = std::filesystem;
namespace ip = boost::interprocess;

namespace omx {

	File::File(std::string path)
		: m_filePath(std::move(path))
	{}

	void File::read(const size_t offset, const size_t size, std::string& dst) {
		if (!fs::exists(m_filePath)) {
			throw std::runtime_error("file not exist");
		}

		if (offset + size > fs::file_size(m_filePath)) {
			throw std::runtime_error("invalid size and offset");
		}

		const ip::mode_t mode = ip::read_only;

		ip::file_mapping mapping(m_filePath.c_str(), mode);

		ip::mapped_region region(mapping, mode, offset, size);

		dst.resize(size);

		const auto* address = reinterpret_cast<const char*>(region.get_address());
		char* destination = dst.data();

		std::copy(address, address + size, destination);
	}

	void File::write(size_t offset, const std::string& src) {
		if (!fs::exists(m_filePath)) {
			create();
		}

		size_t fileSize = fs::file_size(m_filePath);
		if (offset + src.size() > fileSize) {
			fs::resize_file(m_filePath, offset + src.size());
		}

		const ip::mode_t mode = ip::read_write;

		ip::file_mapping mapping(m_filePath.c_str(), mode);

		ip::mapped_region region(mapping, mode, offset, src.size());

		auto* address = reinterpret_cast<char*>(region.get_address());
		const char* source = src.data();

		std::copy(source, source + src.size(), address);
	}

	void File::append(const std::string& src) {
		std::error_code error;
		size_t offset = fs::file_size(m_filePath, error);

		if (error) {
			offset = 0;
		}

		write(offset, src);
	}

	void File::create() {
		fs::path basedir = m_filePath.parent_path();
		if (!fs::exists(basedir)) {
			throw std::runtime_error("invalid directory");
		}

		std::ofstream file(m_filePath, std::ios::binary | std::ios::out);
	}
}
