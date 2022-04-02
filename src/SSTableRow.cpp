#include "SSTableRow.h"

#include <sstream>
#include <cassert>
#include <utility>

namespace omx {

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

	SSTableRow::SSTableRow(Key key, std::string value, EntryType entryType)
		: m_key(key), m_value(std::move(value)), m_entryType(entryType)
	{}

	SSTableRow::SSTableRow(Key key, std::string value)
		: m_key(key), m_value(std::move(value)), m_entryType(EntryType::Put)
	{}

	SSTableRow::SSTableRow(Key key)
		: m_key(key), m_value(), m_entryType(EntryType::Remove)
	{}

	EntryType SSTableRow::getOperationType() const {
		return m_entryType;
	}

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

	std::string serialize(SSTableRowPtr row) {
		std::ostringstream stream;
		const Key key = row->getKey();
		const std::string& value = row->getData();
		const size_t keyLength = sizeof(key.id);
		const size_t numBytes = value.size();
		auto op = static_cast<uint8_t>(row->getOperationType());

		stream.write(reinterpret_cast<const char*>(&op), sizeof(op));
		stream.write(reinterpret_cast<const char*>(&keyLength), sizeof(keyLength));
		stream.write(reinterpret_cast<const char*>(&key.id), keyLength);
		stream.write(reinterpret_cast<const char*>(&numBytes), sizeof(numBytes));
		stream.write(value.data(), numBytes);

		return stream.str();
	}

}