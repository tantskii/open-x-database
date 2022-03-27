#include "Entry.h"

#include <sstream>
#include <cassert>

namespace omx {

	size_t Entry::serialize(std::ostream& os) const {
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

		os.flush();

		return totalBytes;
	}

	size_t Entry::deserialize(std::istream& is) {
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

	Key Entry::getKey() const {
		return m_key;
	}

	const std::string& Entry::getData() const {
		return m_value;
	}

	std::string& Entry::getData() {
		return m_value;
	}

	Entry::Entry(Key key, std::string value)
		: m_key(key), m_value(std::move(value)), m_entryType(EntryType::Put)
	{}

	EntryType Entry::getOperationType() const {
		return m_entryType;
	}

	Entry::Entry(Key key)
		: m_key(key), m_value(), m_entryType(EntryType::Remove)
	{}

}