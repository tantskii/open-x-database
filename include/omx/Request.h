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
		explicit Request(
			RequestType requestType_ = RequestType::Get,
			Key key_ = {},
			std::string value_ = "");

		[[nodiscard]] std::string serialize() const;

		void deserialize(const std::string& buffer);

		bool operator==(const Request& other) const;

		RequestType requestType;
		Key key;
		std::string value;
	};

}