#pragma once

#include "Key.h"
#include "Options.h"

#include <string>

namespace omx {

	enum class RequestType : uint8_t {
		Get = 0,
		Put,
		Delete
	};

	struct Request {
		Request(RequestType requestType_, Key key_, std::string value_ = "")
			: requestType(requestType_), key(key_), value(std::move(value_))
		{}

		RequestType requestType;
		Key key;
		std::string value;
	};

	enum class ResponseStatus : uint8_t {
		Ok = 0,
		NotFound,
		InvalidRequestType,
		UnknownError
	};

	struct Response {
		ResponseStatus responseStatus;
		std::string value;
	};

}