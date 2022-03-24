#include "Entry.h"

#include <sstream>
#include <cassert>

namespace omx {

	void Entry::serialize(std::ostream& os) const {
		const size_t keyLength = sizeof(m_key.id);
		const size_t numBytes = m_bytes.bytesSize();
		auto op = static_cast<uint8_t>(m_op);

		os.write(reinterpret_cast<const char*>(&op), sizeof(op));
		os.write(reinterpret_cast<const char*>(&keyLength), sizeof(keyLength));
		os.write(reinterpret_cast<const char*>(&m_key.id), keyLength);
		os.write(reinterpret_cast<const char*>(&numBytes), sizeof(numBytes));
		os.write(m_bytes.toCString(), numBytes);
	}

	void Entry::deserialize(std::istream& is) {
		size_t keyLength = 0;
		size_t numBytes = 0;
		std::string& data = m_bytes.toString();

		is.read(reinterpret_cast<char*>(&m_op), sizeof(m_op));
		is.read(reinterpret_cast<char*>(&keyLength), sizeof(keyLength));
		is.read(reinterpret_cast<char*>(&m_key.id), keyLength);
		is.read(reinterpret_cast<char*>(&numBytes), sizeof(size_t));

		data.resize(numBytes);
		is.read(data.data(), numBytes);
	}

	Key Entry::getKey() const {
		return m_key;
	}

	const Bytes& Entry::getBytes() const {
		return m_bytes;
	}

	Bytes& Entry::getBytes() {
		return m_bytes;
	}

	Entry::Entry(Key key, Bytes value, Operation operation)
		: m_key(key)
		, m_bytes(std::move(value))
		, m_op(operation)
	{
		if (m_op == Operation::Remove) {
			assert(m_bytes.empty());
		}
	}

	Operation Entry::getOperationType() const {
		return m_op;
	}

	Entry::Entry(Key key)
		: m_key(key)
		, m_bytes()
		, m_op(Operation::Remove)
	{}

}