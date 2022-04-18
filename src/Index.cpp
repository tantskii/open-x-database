#include "Index.h"

#include <fstream>
#include <set>

namespace fs = std::filesystem;

namespace omx {

	SearchHint::SearchHint(uint32_t fileId_, uint32_t offset_, uint32_t size_)
		: fileId(fileId_), offset(offset_), size(size_)
	{}

	void Index::insert(Key key, SearchHint hint) {
		auto it = m_map.find(hint.fileId);

		if (it == m_map.end()) {
			auto tableIndex = std::make_unique<SSTableIndex>(hint.fileId);

			tableIndex->insert(key, FileSearchHint(hint.offset, hint.size));

			update(std::move(tableIndex), true);
		} else {
			auto& tableIndex = *it->second;

			tableIndex->insert(key, FileSearchHint(hint.offset, hint.size));
		}
	}

	void Index::update(SSTableIndexPtr tableIndex, bool trust) {
		if (!trust) {
			auto it = m_map.find(tableIndex->getFileId());
			if (it != m_map.end()) {
				throw std::runtime_error("file id is already indexed");
			}
		}

		m_tableIndexes.push_front(std::move(tableIndex));

		auto it = m_tableIndexes.begin();

		m_map.insert({(*it)->getFileId(), it});
	}

	void Index::update(SSTableIndexPtr tableIndex) {
		update(std::move(tableIndex), false);
	}

	bool Index::get(Key key, SearchHint& hint) const {
		auto fileHint = FileSearchHint{};

		for (const auto& tableIndex: m_tableIndexes) {
			if (tableIndex->get(key, fileHint)) {
				hint.fileId = tableIndex->getFileId();
				hint.offset = fileHint.offset;
				hint.size   = fileHint.size;

				return true;
			}
		}

		hint = SearchHint{};
		return false;
	}

	void Index::load(const std::filesystem::path& dir) {
		if (!fs::exists(dir) || !fs::is_directory(dir)) {
			throw std::runtime_error("invalid directory: " + dir.string());
		}

		std::set<fs::path> sortedIndexFiles;

		for (const fs::directory_entry& directoryEntry: fs::directory_iterator(dir)) {
			if (!directoryEntry.is_regular_file()) {
				continue;
			}

			sortedIndexFiles.insert(directoryEntry.path());
		}

		for (const auto& path: sortedIndexFiles) {
			auto stream = std::ifstream(path, std::ios::binary | std::ios::in);
			if (!stream.is_open() || stream.bad()) {
				throw std::runtime_error("bad stream: " + path.string());
			}

			auto indexTable = std::make_unique<SSTableIndex>();

			indexTable->load(stream);

			update(std::move(indexTable));
		}
	}
}
