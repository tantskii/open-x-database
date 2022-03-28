#pragma once

#include <omx/Key.h>

#include <sstream>
#include <string>
#include <memory>

namespace omx {

	enum class EntryType : uint8_t {
		Put = 0,
		Remove = 1,
	};

	class SSTableRow {
	public:
		SSTableRow(Key key, std::string value);

		explicit SSTableRow(Key key = Key());

		size_t serialize(std::ostream& os) const;

		size_t deserialize(std::istream& is);

		[[nodiscard]] Key getKey() const;

		[[nodiscard]] const std::string& getData() const;

		std::string& getData();

		[[nodiscard]] EntryType getOperationType() const;

		std::size_t getRowSize() const;

	private:
		Key m_key;
		std::string m_value;
		EntryType m_entryType;
	};

	using SSTableRowPtr = std::shared_ptr<SSTableRow>;

}

