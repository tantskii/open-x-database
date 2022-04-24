#include <omx/Request.h>

#include <sstream>
#include "omx/Response.h"


namespace omx {

	omx::Response::Response(ResponseStatus responseStatus_, std::string value_)
		: responseStatus(responseStatus_), value(std::move(value_))
	{}

	std::string Response::serialize() const {
		auto stream = std::ostringstream(std::ios::binary | std::ios::out);

		const uint32_t valueSize = value.size();

		stream.write(reinterpret_cast<const char*>(&responseStatus), sizeof(responseStatus));
		stream.write(reinterpret_cast<const char*>(&valueSize), sizeof(valueSize));
		stream.write(value.c_str(), valueSize);

		return stream.str();
	}

	void Response::deserialize(const std::string& buffer) {
		auto stream = std::istringstream(buffer, std::ios::binary | std::ios::in);

		uint32_t valueSize = 0;

		stream.read(reinterpret_cast<char*>(&responseStatus), sizeof(responseStatus));
		stream.read(reinterpret_cast<char*>(&valueSize), sizeof(valueSize));

		value.resize(valueSize);
		stream.read(value.data(), valueSize);
	}

	bool Response::operator==(const Response& other) const {
		return responseStatus == other.responseStatus && value == other.value;
	}

}
