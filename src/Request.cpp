#include <omx/Request.h>

#include <sstream>

namespace omx {

	Request::Request(RequestType requestType_, Key key_, std::string value_)
		: requestType(requestType_), key(key_), value(std::move(value_))
	{}

	std::string Request::serialize() const {
		auto stream = std::ostringstream(std::ios::binary | std::ios::out);

		const uint32_t valueSize = value.size();

		stream.write(reinterpret_cast<const char*>(&requestType), sizeof(requestType));
		stream.write(reinterpret_cast<const char*>(&key), sizeof(key));
		stream.write(reinterpret_cast<const char*>(&valueSize), sizeof(valueSize));
		stream.write(value.c_str(), valueSize);

		return stream.str();
	}

	void Request::deserialize(const std::string& buffer) {
		auto stream = std::istringstream(buffer, std::ios::binary | std::ios::in);

		uint32_t valueSize = 0;

		stream.read(reinterpret_cast<char*>(&requestType), sizeof(requestType));
		stream.read(reinterpret_cast<char*>(&key), sizeof(key));
		stream.read(reinterpret_cast<char*>(&valueSize), sizeof(valueSize));

		value.resize(valueSize);
		stream.read(value.data(), valueSize);
	}

	bool Request::operator==(const Request& other) const {
		return requestType == other.requestType && key == other.key && value == other.value;
	}
}
