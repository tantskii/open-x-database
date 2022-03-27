#pragma once

#include <omx/Key.h>

#include <sstream>
#include <string>

namespace omx {

	enum class EntryType : uint8_t {
		Put = 0,
		Remove = 1,
	};

	class Entry {
	public:
		Entry(Key key, std::string value);

		explicit Entry(Key key = Key());

		size_t serialize(std::ostream& os) const;

		size_t deserialize(std::istream& is);

		[[nodiscard]] Key getKey() const;

		[[nodiscard]] const std::string& getData() const;

		std::string& getData();

		[[nodiscard]] EntryType getOperationType() const;

	private:
		Key m_key;
		std::string m_value;
		EntryType m_entryType;
	};

}

