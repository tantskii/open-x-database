#pragma once

#include <omx/Key.h>

#include "SSTableIndex.h"

#include <unordered_map>
#include <unordered_set>
#include <list>
#include <memory>
#include <filesystem>

namespace omx {

	struct SearchHint {
		explicit SearchHint(uint32_t fileId = 0, uint32_t offset = 0, uint32_t size = 0);

		uint32_t fileId;
		uint32_t offset;
		uint32_t size;
	};

	class Index {
	public:
		void insert(Key key, SearchHint hint);

		void update(SSTableIndexPtr tableIndex);

		bool get(Key key, SearchHint& hint) const;

		void load(const std::filesystem::path& dir);

	private:
		void update(SSTableIndexPtr tableIndex, bool trust);

		std::list<SSTableIndexPtr> m_tableIndexes;
		std::unordered_map<uint32_t, std::list<SSTableIndexPtr>::const_iterator> m_map;
	};

	using IndexPtr = std::unique_ptr<Index>;
}
