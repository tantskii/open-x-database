#pragma once

#include <omx/Key.h>

#include "SSTable.h"

#include <unordered_map>
#include <memory>
#include <fstream>

namespace omx {

	struct FileSearchHint {
		explicit FileSearchHint(uint32_t offset = 0, uint32_t size = 0);

		uint32_t offset;
		uint32_t size;
	};

	class SSTableIndex {
	public:
		explicit SSTableIndex(uint32_t fileId = 0);

		SSTableIndex(uint32_t fileId, const SSTable& table);

		void insert(Key key, FileSearchHint hint);

		bool get(Key key, FileSearchHint& hint) const;

		uint32_t getFileId() const;

		void dump(std::ofstream& stream) const;

		void load(std::ifstream& stream);

	private:
		std::unordered_map<Key, FileSearchHint, KeyHasher> m_map;
		uint32_t m_fileId;
	};

	using SSTableIndexPtr = std::unique_ptr<SSTableIndex>;
}
