#include <iostream>
#include <memory>
#include <cassert>

#include <omx/Database.h>

int main() {
	// set root database directory
	std::string root = "/tmp/db";

	omx::Options options;
	// set max memory table size to 1mb
	options.maxMemTableSize = 1 * 1024 * 1024;
	// set write ahead log buffer size 1kb;
	// note: this amount of data will be lost in case of server fault
	options.maxWalBufferSize = 1 * 1024;
	// set hashing algorithm for calculating checksums
	options.hashType = omx::HashType::CityHash128;
	// set compression algorithm for optimize space capacity
	options.compressionType = omx::CompressionType::LZ4;

	// create database object
	// note: it is recommended to wrap omx::Database into smart pointer
	auto database = std::make_shared<omx::Database>();

	// open database in root directory
	database->open(root.c_str(), options);

	// create key with id == 1234
	omx::Key key(1234);
	// create string value
	std::string input = "test read write string";

	// store key and value in database
	database->put(key, input);

	// create string object to store output value
	std::string output;

	// read value by passed key
	bool status = database->get(key, output);

	// assert that value was found by passed key
	assert(status);

	// print input and output values
	std::cout << "Input  value: " << input << std::endl;
	std::cout << "Output value: " << output << std::endl;

	// lets remove value
	database->remove(key);

	// try to read removed value by passed key
	status = database->get(key, output);

	// assert that value was NOT found by passed key
	assert(!status);

	return 0;
}