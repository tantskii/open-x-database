#include <iostream>

#include "DatabaseConnection.h"

std::ostream& operator<<(std::ostream& os, const omx::Request& request) {
	os << "Request:"
	   << " type = " << static_cast<int>(request.requestType)
	   << " key = " << request.key.id
	   << " content = '" << request.value << "'";
	return os;
}

std::ostream& operator<<(std::ostream& os, const omx::Response& response) {
	os << "Response: "
		<< " status = " << static_cast<int>(response.responseStatus)
		<< " content = '" << response.value << "'";
	return os;
}

int main() {
	const uint16_t port = 3132;
	const std::string address = "127.0.0.1";

	omx::DatabaseConnection databaseConnection(address, port);

	{
		auto request = omx::Request(omx::RequestType::Put, omx::Key(123), "test string");
		std::cout << request << std::endl;

		omx::Response response = databaseConnection.execute(request).get();

		std::cout << response << std::endl;
	}

	{
		auto request = omx::Request(omx::RequestType::Get, omx::Key(123), "");
		std::cout << request << std::endl;

		omx::Response response = databaseConnection.execute(request).get();

		std::cout << response << std::endl;
	}

}
