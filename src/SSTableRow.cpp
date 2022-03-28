#include "SSTableRow.h"

#include <sstream>
#include <cassert>

namespace omx {

	size_t SSTableRow::serialize(std::ostream& os, bool flush) const {
		const size_t keyLength = sizeof(m_key.id);
		const size_t numBytes = m_value.size();
		auto op = static_cast<uint8_t>(m_entryType);
		size_t totalBytes = 0;

		os.write(reinterpret_cast<const char*>(&op), sizeof(op));
		totalBytes += sizeof(op);
		os.write(reinterpret_cast<const char*>(&keyLength), sizeof(keyLength));
		totalBytes += sizeof(keyLength);
		os.write(reinterpret_cast<const char*>(&m_key.id), keyLength);
		totalBytes += keyLength;
		os.write(reinterpret_cast<const char*>(&numBytes), sizeof(numBytes));
		totalBytes += sizeof(numBytes);
		os.write(m_value.data(), numBytes);
		totalBytes += numBytes;

		if (flush)
			os.flush();

		return totalBytes;
	}

	size_t SSTableRow::deserialize(std::istream& is) {
		size_t keyLength = 0;
		size_t numBytes = 0;
		size_t totalBytes = 0;

		is.read(reinterpret_cast<char*>(&m_entryType), sizeof(m_entryType));
		totalBytes += sizeof(m_entryType);
		is.read(reinterpret_cast<char*>(&keyLength), sizeof(keyLength));
		totalBytes += sizeof(keyLength);
		is.read(reinterpret_cast<char*>(&m_key.id), keyLength);
		totalBytes += keyLength;
		is.read(reinterpret_cast<char*>(&numBytes), sizeof(size_t));
		totalBytes += sizeof(size_t);

		m_value.resize(numBytes);
		is.read(m_value.data(), numBytes);
		totalBytes += numBytes;

		return totalBytes;
	}

	Key SSTableRow::getKey() const {
		return m_key;
	}

	const std::string& SSTableRow::getData() const {
		return m_value;
	}

	std::string& SSTableRow::getData() {
		return m_value;
	}

	SSTableRow::SSTableRow(Key key, std::string value)
		: m_key(key), m_value(std::move(value)), m_entryType(EntryType::Put)
	{}

	EntryType SSTableRow::getOperationType() const {
		return m_entryType;
	}

	SSTableRow::SSTableRow(Key key)
		: m_key(key), m_value(), m_entryType(EntryType::Remove)
	{}

	std::size_t SSTableRow::getRowSize() const {
		const size_t keyLength = sizeof(m_key.id);
		const size_t numBytes = m_value.size();
		size_t totalBytes = 0;

		totalBytes += sizeof(m_entryType);
		totalBytes += sizeof(keyLength);
		totalBytes += keyLength;
		totalBytes += sizeof(numBytes);
		totalBytes += numBytes;

		return totalBytes;
	}

}