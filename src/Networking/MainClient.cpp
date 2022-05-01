#include <omx/DatabaseConnector.h>

#include <boost/program_options.hpp>

#include <iostream>

std::string getResponseStatus(omx::ResponseStatus status) {
	switch (status) {
		case omx::ResponseStatus::Ok:
			return "OK";
		case omx::ResponseStatus::NotFound:
			return "NotFound";
		case omx::ResponseStatus::InvalidRequestType:
			return "InvalidRequestType";
		case omx::ResponseStatus::UnknownError:
			return "UnknownError";
		default:
			return "UnknownResponseStatus";
	}
}

int main(int argc, const char **argv) {
	namespace po = boost::program_options;

	try {
		po::options_description description;
		description.add_options()
			("help", "produce help message")
			("port", po::value<uint16_t>()->default_value(3333), "port number")
			("address", po::value<std::string>()->default_value("127.0.0.1"), "OMXDB server address")
			("request", po::value<int>(), "OMXDB request type: GET = 0, PUT = 1, DELETE = 2")
			("key", po::value<size_t>())
			("value", po::value<std::string>()->default_value(""));

		po::variables_map args;
		po::store(po::parse_command_line(argc, argv, description), args);
		po::notify(args);

		if (args.count("help")) {
			std::cout << description << "\n";
			return 1;
		}

		const auto port        = args["port"].as<uint16_t>();
		const auto address     = args["address"].as<std::string>();
		const auto requestType = static_cast<omx::RequestType>(args["request"].as<int>());
		const auto keyId       = args["key"].as<size_t>();
		const auto value       = args["value"].as<std::string>();

		const auto connector = std::make_shared<omx::DatabaseConnector>(address, port);

		const auto request = omx::Request(requestType, omx::Key(keyId), value);

		const auto response = connector->execute(request).get();

		std::cout << "Response status: " << getResponseStatus(response.responseStatus) << std::endl;
		if (!response.value.empty()) {
			std::cout << "Content        : " << response.value << std::endl;
		}
	} catch (std::exception& ex) {
		std::cerr << ex.what() << std::endl;
		return 1;
	}
}
