#pragma once

#include "Key.h"
#include "Options.h"

#include <string>

namespace omx {

	enum class ResponseStatus : uint8_t {
		Ok = 0,
		NotFound,
		InvalidRequestType,
		UnknownError
	};

	struct Response {
		explicit Response(
			ResponseStatus responseStatus = ResponseStatus::UnknownError,
			std::string value = "");

		[[nodiscard]] std::string serialize() const;

		void deserialize(const std::string& buffer);

		bool operator==(const Response& other) const;

		ResponseStatus responseStatus;
		std::string value;
	};
}
