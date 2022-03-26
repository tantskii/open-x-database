#include <omx/BinaryStorage.h>

#include "../src/Entry.h"
#include "../src/MemTable.h"

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
	omx::Entry input(key, value);
	omx::Entry output;

	std::ostringstream os;
	size_t numWriteBytes = input.serialize(os);
	std::istringstream is(os.str());
	size_t numReadBytes = output.deserialize(is);

	ASSERT_EQ(numReadBytes, numWriteBytes);
	ASSERT_EQ(input.getOperationType(), output.getOperationType());
	ASSERT_EQ(input.getKey().id, output.getKey().id);
	ASSERT_EQ(input.getBytes().toString(), output.getBytes().toString());
}

TEST(MemTable, InsertAndGet) {
	omx::MemTable table;

	omx::Key key1(1);
	omx::Bytes inp1;
	omx::Bytes out1;
	std::string data1 = "1";
	inp1.from(data1);

	omx::Key key2(2);
	omx::Bytes inp2;
	omx::Bytes out2;
	std::string data2 = "2";
	inp2.from(data2);

	omx::Key key3(3);
	omx::Bytes inp3;
	omx::Bytes out3;
	std::string data3 = "3";
	inp3.from(data3);

	table.put(key1, inp1);
	table.put(key2, inp2);
	table.put(key3, inp3);

	table.get(key1, out1);
	table.get(key2, out2);
	table.get(key3, out3);

	ASSERT_EQ(inp1.toString(), out1.toString());
	ASSERT_EQ(inp2.toString(), out2.toString());
	ASSERT_EQ(inp3.toString(), out3.toString());
}

TEST(Index, ReadWrite) {
	omx::Index index;
	omx::SearchHint hint;

	index.insert(omx::Key(1), omx::SearchHint(1, 100));
	index.insert(omx::Key(2), omx::SearchHint(1, 200));
	index.insert(omx::Key(3), omx::SearchHint(1, 300));
	index.insert(omx::Key(3), omx::SearchHint(2, 400));

	ASSERT_TRUE(index.get(omx::Key(1), hint));
	ASSERT_EQ(hint.fileId, 1);
	ASSERT_EQ(hint.offset, 100);

	ASSERT_TRUE(index.get(omx::Key(2), hint));
	ASSERT_EQ(hint.fileId, 1);
	ASSERT_EQ(hint.offset, 200);

	ASSERT_TRUE(index.get(omx::Key(3), hint));
	ASSERT_EQ(hint.fileId, 2);
	ASSERT_EQ(hint.offset, 400);

	ASSERT_FALSE(index.get(omx::Key(4), hint));
	ASSERT_EQ(hint.fileId, 0);
	ASSERT_EQ(hint.offset, 0);
}

TEST(Index, Merge) {
	omx::Index index_0;
	omx::Index index_1;
	omx::SearchHint hint;

	index_0.insert(omx::Key(1), omx::SearchHint(1, 100));
	index_0.insert(omx::Key(2), omx::SearchHint(1, 200));
	index_0.insert(omx::Key(3), omx::SearchHint(1, 300));
	index_1.insert(omx::Key(3), omx::SearchHint(2, 400));
	index_1.insert(omx::Key(4), omx::SearchHint(2, 500));
	index_0.merge(index_1);

	ASSERT_TRUE(index_0.get(omx::Key(1), hint));
	ASSERT_EQ(hint.fileId, 1);
	ASSERT_EQ(hint.offset, 100);

	ASSERT_TRUE(index_0.get(omx::Key(2), hint));
	ASSERT_EQ(hint.fileId, 1);
	ASSERT_EQ(hint.offset, 200);

	ASSERT_TRUE(index_0.get(omx::Key(3), hint));
	ASSERT_EQ(hint.fileId, 2);
	ASSERT_EQ(hint.offset, 400);

	ASSERT_TRUE(index_0.get(omx::Key(4), hint));
	ASSERT_EQ(hint.fileId, 2);
	ASSERT_EQ(hint.offset, 500);
}

TEST(MemTable, InsertSameKey) {
	omx::MemTable table;

	omx::Key key(1234);
	omx::Bytes inp;
	omx::Bytes out;
	std::string data = "a";

	inp.from(data);
	table.put(key, inp);

	data += "a";
	inp.from(data);
	table.put(key, inp);

	data += "a";
	inp.from(data);
	table.put(key, inp);

	ASSERT_TRUE(table.get(key, out));

	ASSERT_EQ(inp.toString(), out.toString());
}

TEST(MemTable, Remove) {
	omx::MemTable table;

	omx::Key key(1234);
	omx::Bytes inp;
	omx::Bytes out;
	std::string data = "a";

	inp.from(data);
	table.put(key, inp);

	data += "a";
	inp.from(data);
	table.put(key, inp);

	data += "a";
	inp.from(data);
	table.put(key, inp);

	table.remove(key);

	ASSERT_FALSE(table.get(key, out));

	ASSERT_TRUE(out.empty());
}

TEST(MemTable, Dump) {
	omx::MemTable table;
	omx::Entry entry;
	omx::Bytes inp;
	omx::Index index;
	omx::SearchHint hint;
	std::string data;
	std::string buffer;

	data = "111111111111111111111111111111111111111111111111111111";
	inp.from(data);
	table.put(omx::Key(1), inp);

	data = "222222222222222222222222222222222222222222222222222222";
	inp.from(data);
	table.put(omx::Key(2), inp);

	data = "333333333333333333333333333333333333333333333333333333";
	inp.from(data);
	table.put(omx::Key(3), inp);

	table.remove(omx::Key(1));

	std::ostringstream os;
	table.dump(0, os, index);

	data = os.str();

	ASSERT_TRUE(index.get(omx::Key(2), hint));
	buffer = data.substr(hint.offset, hint.size);
	std::istringstream is1(buffer);
	entry.deserialize(is1);

	ASSERT_EQ(entry.getOperationType(), omx::EntryType::Put);
	ASSERT_EQ(entry.getBytes().toString(), "222222222222222222222222222222222222222222222222222222");
	ASSERT_EQ(entry.getKey(), omx::Key(2));

	ASSERT_TRUE(index.get(omx::Key(1), hint));
	buffer = data.substr(hint.offset, hint.size);
	std::istringstream is2(buffer);
	entry.deserialize(is2);

	ASSERT_EQ(entry.getOperationType(), omx::EntryType::Remove);
	ASSERT_TRUE(entry.getBytes().empty());
	ASSERT_EQ(entry.getKey(), omx::Key(1));
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

