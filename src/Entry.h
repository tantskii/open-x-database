#pragma once

#include <omx/Key.h>
#include <omx/Bytes.h>

#include <sstream>

namespace omx {

	enum class EntryType : uint8_t {
		Put = 0,
		Remove = 1,
	};

	class Entry {
	public:
		Entry(Key key, Bytes value);
		explicit Entry(Key key = Key());

		void serialize(std::ostream& os) const;

		void deserialize(std::istream& is);

		[[nodiscard]] Key getKey() const;

		[[nodiscard]] const Bytes& getBytes() const;

		Bytes& getBytes();

		[[nodiscard]] EntryType getOperationType() const;

	private:
		Key m_key;
		Bytes m_bytes;
		EntryType m_entryType;
	};

}

