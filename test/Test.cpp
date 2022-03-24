#include <omx/BinaryStorage.h>

#include "../src/Entry.h"

#include <gtest/gtest.h>

#include <vector>
#include <filesystem>
#include <sstream>

namespace fs = std::filesystem;

#define CLEAR_DIR(__dir) \
	{ std::filesystem::remove_all(__dir); std::filesystem::create_directories(__dir); }

fs::path temp_dir = fs::temp_directory_path() / "image_storage";

constexpr size_t kb(size_t bytes) {
	return bytes * 1024;
}

constexpr size_t mb(size_t bytes) {
	return bytes * 1024 * 1024;
}

constexpr size_t gb(size_t bytes) {
	return bytes * 1024 * 1024 * 1024;
}

TEST(Entry, Get) {
	omx::Key key(1234);
	omx::Bytes value;
	std::string data = "test read write string";
	value.from(data);
	omx::Entry entry(key, value, omx::Operation::Read);

	omx::Key key2(1234);
	omx::Bytes value2;
	omx::Entry entryOutput(key2, value2, omx::Operation::Remove);

	std::ostringstream os;
	entry.serialize(os);
	std::istringstream is(os.str());
	entryOutput.deserialize(is);

	ASSERT_EQ(entry.getOperationType(), entryOutput.getOperationType());
	ASSERT_EQ(entry.getKey().id, entryOutput.getKey().id);
	ASSERT_EQ(entry.getBytes().toString(), entryOutput.getBytes().toString());
}

TEST(BinaryStorage, Create) {
	CLEAR_DIR(temp_dir);

	omx::Options options;
	omx::BinaryStorage db;

	options.setBlockSize(kb(4));
	options.setMaxFileSize(gb(1));
	options.setMaxOpenFiles(1000);
	options.setWriteBufferSize(mb(4));
	options.setBlockCacheSize(mb(8));

	ASSERT_NO_THROW(db.open(temp_dir.c_str(), options));
}

TEST(BinaryStorage, String) {
	CLEAR_DIR(temp_dir);

	omx::Key key(1234);
	omx::BinaryStorage db;
	omx::Bytes input;
	omx::Bytes output;
	std::string str = "test read write string";

	input.from(str);

	ASSERT_NO_THROW(db.open(temp_dir.c_str()));
	ASSERT_NO_THROW(db.put(key, input));
	ASSERT_NO_THROW(db.get(key, output));
	ASSERT_EQ(input.toString(), output.toString());
	ASSERT_NO_THROW(db.remove(key));
	ASSERT_THROW(db.get(key, output), std::runtime_error);
}

TEST(BinaryStorage, VectorInt) {
	CLEAR_DIR(temp_dir);

	omx::Key key(1234);
	omx::BinaryStorage db;
	omx::Bytes input;
	omx::Bytes output;
	std::vector<int> nums = {123, 909, 1323};

	input.from(nums);

	ASSERT_NO_THROW(db.open(temp_dir.c_str()));
	ASSERT_NO_THROW(db.put(key, input));
	ASSERT_NO_THROW(db.get(key, output));

	const int* numsArr = output.as<int>();
	const size_t numsSize = output.bytesSize();

	ASSERT_EQ(numsSize, nums.size() * sizeof(int));
	for (size_t i = 0; i < numsSize; i += sizeof(int)) {
		ASSERT_EQ(numsArr[i / sizeof(int)], nums[i / sizeof(int)]);
	}
}

TEST(Transaction, PutRemove) {
	CLEAR_DIR(temp_dir);

	omx::BinaryStorage db;
	omx::Transaction transaction;
	omx::Bytes input0;
	omx::Bytes input1;
	omx::Bytes input2;
	omx::Bytes output0;
	omx::Bytes output1;
	omx::Bytes output2;
	omx::Key key0(0);
	omx::Key key1(1);
	omx::Key key2(2);
	std::string str0 = "Hello";
	std::string str1 = "World";
	std::string str2 = "Remove";

	input0.from(str0);
	input1.from(str1);
	input2.from(str2);

	ASSERT_NO_THROW(db.open(temp_dir.c_str()));
	ASSERT_NO_THROW(db.put(key2, input2));
	ASSERT_NO_THROW(transaction.put(key0, input0));
	ASSERT_NO_THROW(transaction.put(key1, input1));
	ASSERT_NO_THROW(transaction.remove(key2));
	ASSERT_NO_THROW(db.execute(transaction));
	ASSERT_NO_THROW(db.get(key0, output0));
	ASSERT_NO_THROW(db.get(key1, output1));
	ASSERT_THROW(db.get(key2, output2), std::runtime_error);

	ASSERT_EQ(input0.toString(), output0.toString());
	ASSERT_EQ(input1.toString(), output1.toString());
}

TEST(Transaction, Clear) {
	CLEAR_DIR(temp_dir);

	omx::BinaryStorage db;
	omx::Transaction transaction;
	omx::Bytes input0;
	omx::Bytes input1;
	omx::Bytes input2;
	omx::Bytes output0;
	omx::Bytes output1;
	omx::Bytes output2;
	omx::Key key0(0);
	omx::Key key1(1);
	omx::Key key2(2);
	std::string str0 = "Hello";
	std::string str1 = "World";
	std::string str2 = "Remove";

	input0.from(str0);
	input1.from(str1);
	input2.from(str2);

	ASSERT_NO_THROW(db.open(temp_dir.c_str()));
	ASSERT_NO_THROW(db.put(key2, input2));
	ASSERT_NO_THROW(transaction.put(key0, input0));
	ASSERT_NO_THROW(transaction.put(key1, input1));
	ASSERT_NO_THROW(transaction.remove(key2));
	ASSERT_NO_THROW(transaction.clear());
	ASSERT_NO_THROW(db.execute(transaction));

	ASSERT_THROW(db.get(key0, output0), std::runtime_error);
	ASSERT_THROW(db.get(key1, output1), std::runtime_error);
	ASSERT_NO_THROW(db.get(key2, output2));

	ASSERT_EQ(input2.toString(), output2.toString());
}

TEST(Transaction, RemoveNotExistedKey) {
	CLEAR_DIR(temp_dir);

	omx::BinaryStorage db;
	omx::Transaction transaction;
	omx::Key key(1234);

	ASSERT_NO_THROW(db.open(temp_dir.c_str()));

	ASSERT_NO_THROW(transaction.remove(key));
	ASSERT_NO_THROW(db.execute(transaction));
	ASSERT_NO_THROW(db.remove(key));
}

