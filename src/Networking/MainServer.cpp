#include <omx/DatabaseServer.h>

#include <boost/program_options.hpp>

#include <iostream>

int main(int argc, const char **argv) {
	namespace po = boost::program_options;

	try {
		po::options_description description;
		description.add_options()
			("help", "produce help message")
			("port", po::value<uint16_t>()->default_value(3333), "port number")
			("threads", po::value<uint16_t>()->default_value(1), "number of threads")
			("db-directory", po::value<std::string>()->default_value("/tmp/omxdb"),
			 	"OMXDB root directory. NOTE: directory should exist.");

		po::variables_map args;
		po::store(po::parse_command_line(argc, argv, description), args);
		po::notify(args);

		if (args.count("help")) {
			std::cout << description << "\n";
			return 1;
		}

		const auto port              = args["port"].as<uint16_t>();
		const auto numThreads        = args["threads"].as<uint16_t>();
		const auto databaseDirectory = args["db-directory"].as<std::string>();

		auto server = omx::DatabaseServer(port, numThreads, databaseDirectory);

		server.start();
		server.wait();
	} catch (std::exception& ex) {
		std::cerr << ex.what() << std::endl;
		return 1;
	}
}