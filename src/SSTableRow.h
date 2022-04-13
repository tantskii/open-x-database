#pragma once

#include <omx/Key.h>

#include <sstream>
#include <string>
#include <memory>

using UInt128 = std::pair<uint64_t, uint64_t>;

namespace omx {

	enum class EntryType : uint8_t {
		Put = 0,
		Remove = 1,
	};

	class SSTableRow {
	public:
		SSTableRow(Key key, std::string value, EntryType entryType, UInt128 checksum = {});

		SSTableRow(Key key, std::string value, UInt128 checksum = {});

		explicit SSTableRow(Key key = Key());

		[[nodiscard]] Key getKey() const;

		[[nodiscard]] const std::string& getData() const;

		std::string& getData();

		[[nodiscard]] EntryType getOperationType() const;

		std::size_t getRowSize() const;

		const UInt128& getChecksum() const;

	private:
		Key m_key;
		std::string m_value;
		EntryType m_entryType;
		UInt128 m_checksum = {0, 0};
	};

	using SSTableRowPtr = std::shared_ptr<SSTableRow>;

	std::string serialize(SSTableRowPtr row);

	SSTableRowPtr deserialize(const std::string& data);

	SSTableRowPtr deserialize(std::istream& is);
}

