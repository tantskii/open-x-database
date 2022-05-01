#include "UUID.h"

#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

namespace omx {

	UUID::UUID()
		: m_uuid(boost::uuids::random_generator()())
	{}

	std::ostream& operator<<(std::ostream& stream, const UUID& uuid) {
		stream << "[" + boost::lexical_cast<std::string>(uuid.m_uuid) + "]";
		return stream;
	}
}
