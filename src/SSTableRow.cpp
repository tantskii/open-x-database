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
		const uint8_t sizeOfKey      = sizeof(m_key.id);
		const size_t numBytes        = m_value.size();
		const uint8_t sizeOfChecksum = sizeof(m_checksum);
		size_t totalBytes = 0;

		totalBytes += sizeof(m_entryType);
		totalBytes += sizeof(sizeOfKey);
		totalBytes += sizeOfKey;
		totalBytes += sizeof(numBytes);
		totalBytes += numBytes;
		totalBytes += sizeof(sizeOfChecksum);
		totalBytes += sizeOfChecksum;

		return totalBytes;
	}

	const std::pair<uint64_t, uint64_t>& SSTableRow::getChecksum() const {
		return m_checksum;
	}

	std::string serialize(SSTableRowPtr row) {
		std::ostringstream stream;
		const Key key = row->getKey();
		const std::string& value = row->getData();
		const uint8_t sizeOfKey = sizeof(key.id);
		const size_t numBytes = value.size();
		auto op = static_cast<uint8_t>(row->getOperationType());
		const auto checksum = row->getChecksum();
		const uint8_t sizeOfChecksum = sizeof(checksum);

		stream.write(reinterpret_cast<const char*>(&op), sizeof(op));
		stream.write(reinterpret_cast<const char*>(&sizeOfKey), sizeof(sizeOfKey));
		stream.write(reinterpret_cast<const char*>(&key.id), sizeOfKey);
		stream.write(reinterpret_cast<const char*>(&numBytes), sizeof(numBytes));
		stream.write(value.data(), numBytes);
		stream.write(reinterpret_cast<const char*>(&sizeOfChecksum), sizeof(sizeOfChecksum));
		stream.write(reinterpret_cast<const char*>(&checksum.first), sizeOfChecksum);

		return stream.str();
	}

	SSTableRowPtr deserialize(const std::string& data) {
		std::istringstream stream(data);
		return deserialize(stream);
	}

	SSTableRowPtr deserialize(std::istream& stream) {
		uint8_t sizeOfKey = 0;
		size_t numBytes = 0;
		uint8_t sizeOfChecksum = 0;
		EntryType entryType;
		Key key;
		std::string value;
		UInt128 checksum;

		stream.read(reinterpret_cast<char*>(&entryType), sizeof(entryType));
		stream.read(reinterpret_cast<char*>(&sizeOfKey), sizeof(sizeOfKey));
		stream.read(reinterpret_cast<char*>(&key.id), sizeOfKey);
		stream.read(reinterpret_cast<char*>(&numBytes), sizeof(size_t));

		value.resize(numBytes);
		stream.read(value.data(), numBytes);
		stream.read(reinterpret_cast<char*>(&sizeOfChecksum), sizeof(sizeOfChecksum));
		stream.read(reinterpret_cast<char*>(&checksum.first), sizeOfChecksum);

		return std::make_shared<SSTableRow>(key, value, entryType, checksum);
	}

}