#include <omx/Database.h>

#include <iostream>

int main() {
	// define directory were database will store elements
	std::string temp_dir = "/tmp/db";

	// set options for database
	omx::Options options;
	// set max chunk size to 1 GB
	options.setMaxFileSize(1 * 1024 * 1024 * 1024);
	// set max number of opened files to 100
	options.setMaxOpenFiles(100);
	// set size of buffer that will keep data before dump to the disk to 1 GB
	options.setWriteBufferSize(1 * 1024 * 1024 * 1024);

	// create database object
	omx::BinaryStorage database;
	database.open(temp_dir.c_str());

	// create key
	omx::Key key(1234);

	// prepare string for writing into database
	std::string str = "Hello, world!";
	omx::Bytes input;
	input.from(str);

	// write data
	database.put(key, input);

	// read it
	omx::Bytes output;
	database.get(key, output);

	// assert that we read exact the same string that we wrote
	std::cout << "input : " << input.toString() << std::endl;
	std::cout << "output: " << output.toString() << std::endl;

	// remove string
	database.remove(key);

	// Make write transaction
	omx::Transaction transaction;

	// before transaction executing lets put input back
	database.put(key, input);

	std::string str0 = "Zero";
	omx::Key key0(0);
	omx::Bytes input0;
	input0.from(str0);

	std::string str1 = "One";
	omx::Key key1(1);
	omx::Bytes input1;
	input1.from(str1);

	transaction.put(key0, input0);
	transaction.remove(key);
	transaction.put(key1, input1);
	database.execute(transaction);

	// read data
	omx::Bytes output0;
	omx::Bytes output1;
	database.get(key0, output0);
	database.get(key1, output1);

	std::cout << "key: " << key0.id << " value: " << output0.toString() << std::endl;
	std::cout << "key: " << key1.id << " value: " << output1.toString() << std::endl;

	return 0;
}