#pragma once

#include <ostream>

#include <boost/uuid/uuid.hpp>

namespace omx {

	class UUID {
	public:
		UUID();

		friend std::ostream& operator<<(std::ostream& stream, const UUID& uuid);

	private:
		boost::uuids::uuid m_uuid;
	};



}
