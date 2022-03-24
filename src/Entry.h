#pragma once

#include <omx/Key.h>
#include <omx/Bytes.h>

#include "OperationTypes.h"

#include <sstream>

namespace omx {

	class Entry {
	public:
		Entry(Key key, Bytes value, Operation operation);
		Entry(Key key, Bytes&& value, Operation operation) noexcept;

		void serialize(std::ostream& os) const;

		void deserialize(std::istream& is);

		[[nodiscard]] Key getKey() const;

		[[nodiscard]] const Bytes& getBytes() const;

		Bytes& getBytes();

		[[nodiscard]] Operation getOperationType() const;

	private:
		Key m_key;
		Bytes m_bytes;
		Operation m_op;
	};

}

