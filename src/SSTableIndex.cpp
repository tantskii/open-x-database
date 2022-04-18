#include "SSTableIndex.h"

namespace omx {

	SSTableIndex::SSTableIndex(const uint32_t fileId)
		: m_fileId(fileId)
	{}

	void SSTableIndex::insert(Key key, FileSearchHint hint) {
		m_map.insert_or_assign(key, hint);
	}

	bool SSTableIndex::get(Key key, FileSearchHint& hint) const {
		auto it = m_map.find(key);

		if (it == m_map.end()) {
			hint = FileSearchHint();
			return false;
		}

		hint = it->second;

		return true;
	}

	void SSTableIndex::dump(std::ofstream& stream) const {
		if (!stream.is_open()) {
			throw std::runtime_error("invalid output stream");
		}

		constexpr size_t sizeOfFileId = sizeof(m_fileId);
		constexpr size_t sizeOfKey = sizeof(Key::id);
		constexpr size_t sizeOfHint = sizeof(FileSearchHint);

		stream.write(reinterpret_cast<const char*>(&sizeOfFileId), sizeof(sizeOfFileId));
		stream.write(reinterpret_cast<const char*>(&m_fileId), sizeOfFileId);

		for (const auto& [key, hint]: m_map) {
			stream.write(reinterpret_cast<const char*>(&sizeOfKey), sizeof(sizeOfKey));
			stream.write(reinterpret_cast<const char*>(&key.id), sizeOfKey);
			stream.write(reinterpret_cast<const char*>(&sizeOfHint), sizeof(sizeOfHint));
			stream.write(reinterpret_cast<const char*>(&hint), sizeOfHint);
		}

		stream.flush();
	}

	 void SSTableIndex::load(std::ifstream& stream) {
		if (!stream.is_open()) {
			throw std::runtime_error("invalid input stream");
		}

		size_t sizeOfFileId = 0;
		size_t sizeOfKey    = 0;
		size_t sizeOfHint   = 0;

		auto key  = Key{};
		auto hint = FileSearchHint{};

		stream.read(reinterpret_cast<char*>(&sizeOfFileId), sizeof(sizeOfFileId));
		stream.read(reinterpret_cast<char*>(&m_fileId), sizeOfFileId);

		while (!stream.eof()) {
			stream.read(reinterpret_cast<char*>(&sizeOfKey), sizeof(sizeOfKey));
			stream.read(reinterpret_cast<char*>(&key.id), sizeOfKey);
			stream.read(reinterpret_cast<char*>(&sizeOfHint), sizeof(sizeOfHint));
			stream.read(reinterpret_cast<char*>(&hint), sizeOfHint);
			stream.peek();

			m_map.insert({key, hint});
		}
	}

	uint32_t SSTableIndex::getFileId() const {
		return m_fileId;
	}

	FileSearchHint::FileSearchHint(const uint32_t offset_, const uint32_t size_)
		: offset(offset_), size(size_)
	{}
}