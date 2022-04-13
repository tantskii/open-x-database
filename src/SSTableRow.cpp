#include "SSTableRow.h"

#include <sstream>
#include <cassert>
#include <utility>

namespace omx {

	Key SSTableRow::getKey() const {
		return m_key;
	}

	const std::string& SSTableRow::getData() const {
		return m_value;
	}

	std::string& SSTableRow::getData() {
		return m_value;
	}

	SSTableRow::SSTableRow(Key key, std::string value, EntryType entryType, UInt128 checksum)
		: m_key(key), m_value(std::move(value)), m_entryType(entryType), m_checksum(std::move(checksum))
	{}

	SSTableRow::SSTableRow(Key key, std::string value, UInt128 checksum)
		: m_key(key), m_value(std::move(value)), m_entryType(EntryType::Put), m_checksum(std::move(checksum))
	{}

	SSTableRow::SSTableRow(Key key)
		: m_key(key), m_value(), m_entryType(EntryType::Remove), m_checksum({0, 0})
	{}

	EntryType SSTableRow::getOperationType() const {
		return m_entryType;
	}

	std::size_t SSTableRow::getRowSize() const {
		const size_t keyLength = sizeof(m_key.id);
		const size_t numBytes = m_value.size();
		const size_t checksumLength = sizeof(m_checksum);
		size_t totalBytes = 0;

		totalBytes += sizeof(m_entryType);
		totalBytes += sizeof(keyLength);
		totalBytes += keyLength;
		totalBytes += sizeof(numBytes);
		totalBytes += numBytes;
		totalBytes += sizeof(checksumLength);
		totalBytes += checksumLength;

		return totalBytes;
	}

	const std::pair<uint64_t, uint64_t>& SSTableRow::getChecksum() const {
		return m_checksum;
	}

	std::string serialize(SSTableRowPtr row) {
		std::ostringstream stream;
		const Key key = row->getKey();
		const std::string& value = row->getData();
		const size_t keyLength = sizeof(key.id);
		const size_t numBytes = value.size();
		auto op = static_cast<uint8_t>(row->getOperationType());
		const auto checksum = row->getChecksum();
		const size_t checksumLength = sizeof(checksum);

		stream.write(reinterpret_cast<const char*>(&op), sizeof(op));
		stream.write(reinterpret_cast<const char*>(&keyLength), sizeof(keyLength));
		stream.write(reinterpret_cast<const char*>(&key.id), keyLength);
		stream.write(reinterpret_cast<const char*>(&numBytes), sizeof(numBytes));
		stream.write(value.data(), numBytes);
		stream.write(reinterpret_cast<const char*>(&checksumLength), sizeof(checksumLength));
		stream.write(reinterpret_cast<const char*>(&checksum.first), checksumLength);

		return stream.str();
	}

	SSTableRowPtr deserialize(const std::string& data) {
		std::istringstream stream(data);
		return deserialize(stream);
	}

	SSTableRowPtr deserialize(std::istream& stream) {
		size_t keyLength = 0;
		size_t numBytes = 0;
		size_t checksumLength = 0;
		EntryType entryType;
		Key key;
		std::string value;
		UInt128 checksum;

		stream.read(reinterpret_cast<char*>(&entryType), sizeof(entryType));
		stream.read(reinterpret_cast<char*>(&keyLength), sizeof(keyLength));
		stream.read(reinterpret_cast<char*>(&key.id), keyLength);
		stream.read(reinterpret_cast<char*>(&numBytes), sizeof(size_t));

		value.resize(numBytes);
		stream.read(value.data(), numBytes);
		stream.read(reinterpret_cast<char*>(&checksumLength), sizeof(checksumLength));
		stream.read(reinterpret_cast<char*>(&checksum.first), checksumLength);

		return std::make_shared<SSTableRow>(key, value, entryType, checksum);
	}

}