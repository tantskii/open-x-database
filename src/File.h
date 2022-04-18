#pragma once

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/file_mapping.hpp>

#include <string>
#include <filesystem>

namespace omx {

	class File {
	public:
		explicit File(std::string path);

		void read(size_t offset, size_t size, std::string& dst);

		void write(size_t offset, const std::string& src);

		void append(const std::string& src);

	private:
		void create();

		const std::filesystem::path m_filePath;
	};

	using FilePtr = std::shared_ptr<File>;
}
