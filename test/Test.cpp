#include <omx/BinaryStorage.h>

#include "../src/MemTable.h"
#include "../src/SSTable.h"
#include "../src/SSTableRow.h"
#include "../src/StorageEngine.h"

#include <gtest/gtest.h>

#include <vector>
#include <filesystem>
#include <fstream>
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
	std::string value = "test read write string";
	omx::SSTableRow input(key, value);
	omx::SSTableRow output;

	std::ostringstream os;
	size_t numWriteBytes = input.serialize(os);
	std::istringstream is(os.str());
	size_t numReadBytes = output.deserialize(is);

	ASSERT_EQ(numReadBytes, numWriteBytes);
	ASSERT_EQ(input.getOperationType(), output.getOperationType());
	ASSERT_EQ(input.getKey().id, output.getKey().id);
	ASSERT_EQ(input.getData(), output.getData());
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

TEST(Index, DunpRestore) {
	CLEAR_DIR(temp_dir);

	omx::Index index_0;
	omx::Index index_1;
	omx::SearchHint hint;

	index_0.insert(omx::Key(1), omx::SearchHint(1, 100));
	index_0.insert(omx::Key(2), omx::SearchHint(1, 200));
	index_0.insert(omx::Key(3), omx::SearchHint(1, 300));
	index_0.insert(omx::Key(3), omx::SearchHint(2, 400));

	{
		std::ofstream file(temp_dir / "index.bin", std::ios::binary | std::ios::app);
		index_0.dump(file);
	}

	{
		std::ifstream file(temp_dir / "index.bin", std::ios::binary | std::ios::in);
		index_1.load(file);
	}

	ASSERT_TRUE(index_1.get(omx::Key(1), hint));
	ASSERT_EQ(hint.fileId, 1);
	ASSERT_EQ(hint.offset, 100);

	ASSERT_TRUE(index_1.get(omx::Key(2), hint));
	ASSERT_EQ(hint.fileId, 1);
	ASSERT_EQ(hint.offset, 200);

	ASSERT_TRUE(index_1.get(omx::Key(3), hint));
	ASSERT_EQ(hint.fileId, 2);
	ASSERT_EQ(hint.offset, 400);

	ASSERT_FALSE(index_1.get(omx::Key(4), hint));
	ASSERT_EQ(hint.fileId, 0);
	ASSERT_EQ(hint.offset, 0);
}

TEST(SSTable, Merge) {
	omx::SSTable lhs;
	omx::SSTable rhs;

	lhs.append(std::make_shared<omx::SSTableRow>(omx::Key(0)));         // delete
	lhs.append(std::make_shared<omx::SSTableRow>(omx::Key(0), "0002")); // update
	lhs.append(std::make_shared<omx::SSTableRow>(omx::Key(0), "0001")); // update

	rhs.append(std::make_shared<omx::SSTableRow>(omx::Key(0), "0000")); // put
	rhs.append(std::make_shared<omx::SSTableRow>(omx::Key(1), "0020")); // update
	rhs.append(std::make_shared<omx::SSTableRow>(omx::Key(1), "0010")); // put

	lhs.merge(rhs);
	const auto& rows = lhs.getRowList();

	ASSERT_EQ(rows.size(), 2);
	ASSERT_EQ(rows[0]->getKey(), omx::Key(0));
	ASSERT_EQ(rows[0]->getOperationType(), omx::EntryType::Remove);
	ASSERT_EQ(rows[1]->getKey(), omx::Key(1));
	ASSERT_EQ(rows[1]->getOperationType(), omx::EntryType::Put);
}

TEST(MemTable, InsertAndGet) {
	omx::MemTable table;

	omx::Key key1(1);
	std::string out1;
	std::string inp1 = "1";

	omx::Key key2(2);
	std::string inp2;
	std::string out2 = "2";

	omx::Key key3(3);
	std::string inp3;
	std::string out3 = "3";

	table.put(key1, inp1);
	table.put(key2, inp2);
	table.put(key3, inp3);

	table.get(key1, out1);
	table.get(key2, out2);
	table.get(key3, out3);

	ASSERT_EQ(inp1, out1);
	ASSERT_EQ(inp2, out2);
	ASSERT_EQ(inp3, out3);
}

TEST(MemTable, InsertSameKey) {
	omx::MemTable table;

	omx::Key key(1234);
	std::string out;
	std::string inp;

	table.put(key, inp += "a");
	table.put(key, inp += "a");
	table.put(key, inp += "a");

	ASSERT_TRUE(table.get(key, out));

	ASSERT_EQ(inp, out);
}

TEST(MemTable, Remove) {
	omx::MemTable table;

	omx::Key key(1234);
	std::string out;
	std::string inp;

	table.put(key, inp += "a");
	table.put(key, inp += "a");
	table.put(key, inp += "a");

	table.remove(key);

	ASSERT_FALSE(table.get(key, out));

	ASSERT_TRUE(out.empty());
}

TEST(MemTable, Dump) {
	omx::MemTable table;
	omx::SSTableRow entry;
	omx::Bytes inp;
	omx::Index index;
	omx::SearchHint hint;
	std::string data;
	std::string buffer;

	table.put(omx::Key(1), "111111111111111111111111111111111111111111111111111111");
	table.put(omx::Key(2), "222222222222222222222222222222222222222222222222222222");
	table.put(omx::Key(3), "333333333333333333333333333333333333333333333333333333");
	table.remove(omx::Key(1));

	std::ostringstream os;
	table.dump(0, os, index);

	data = os.str();

	ASSERT_TRUE(index.get(omx::Key(2), hint));
	buffer = data.substr(hint.offset, hint.size);
	std::istringstream is1(buffer);
	entry.deserialize(is1);

	ASSERT_EQ(entry.getOperationType(), omx::EntryType::Put);
	ASSERT_EQ(entry.getData(), "222222222222222222222222222222222222222222222222222222");
	ASSERT_EQ(entry.getKey(), omx::Key(2));

	ASSERT_TRUE(index.get(omx::Key(1), hint));
	buffer = data.substr(hint.offset, hint.size);
	std::istringstream is2(buffer);
	entry.deserialize(is2);

	ASSERT_EQ(entry.getOperationType(), omx::EntryType::Remove);
	ASSERT_TRUE(entry.getData().empty());
	ASSERT_EQ(entry.getKey(), omx::Key(1));
}

TEST(MemTable, Restore) {
	CLEAR_DIR(temp_dir);

	std::string filename = temp_dir / "log.bin";
	std::string value;
	omx::MemTable table;

	table.setWriteAheadLog(filename);
	table.put(omx::Key(1), "111111111111111111111111111111111111111111111111111111");
	table.put(omx::Key(2), "222222222222222222222222222222222222222222222222222222");
	table.put(omx::Key(3), "333333333333333333333333333333333333333333333333333333");
	table.remove(omx::Key(1));

	std::ifstream input(filename, std::ios::binary);

	omx::MemTable restored;
	restored.restoreFromLog(input);

	ASSERT_FALSE(restored.get(omx::Key(1), value));
	ASSERT_TRUE(restored.get(omx::Key(2), value));
	ASSERT_EQ(value, "222222222222222222222222222222222222222222222222222222");
	ASSERT_TRUE(restored.get(omx::Key(3), value));
	ASSERT_EQ(value, "333333333333333333333333333333333333333333333333333333");
}

TEST(StorageEngine, ReadWrite) {
	CLEAR_DIR(temp_dir);

	omx::StorageEngine storage(temp_dir);
	std::string input = "111111111111111111111111111111111111111111111111111111";
	std::string output1;
	std::string output2;
	std::string output3;

	for (int i = 1; i <= 100000; ++i) {
		storage.put(omx::Key(i), input);

		if (i == 10000) {
			storage.remove(omx::Key(10));
		}
	}

	ASSERT_TRUE(storage.get(omx::Key(3), output1));
	ASSERT_EQ(output1, input);
	ASSERT_TRUE(storage.get(omx::Key(99999), output2));
	ASSERT_EQ(output2, input);
	ASSERT_FALSE(storage.get(omx::Key(100001), output3));
	ASSERT_FALSE(storage.get(omx::Key(10), output3));
}

TEST(StorageEngine, Restore) {
	CLEAR_DIR(temp_dir);

	std::string input = "111111111111111111111111111111111111111111111111111111";
	std::string output1;
	std::string output2;
	std::string output3;

	{
		omx::StorageEngine storage(temp_dir);

		for (int i = 1; i <= 100000; ++i) {
			storage.put(omx::Key(i), input);

			if (i == 10000) {
				storage.remove(omx::Key(10));
			}
		}
	}

	omx::StorageEngine storage(temp_dir);

	ASSERT_TRUE(storage.get(omx::Key(3), output1));
	ASSERT_EQ(output1, input);
	ASSERT_TRUE(storage.get(omx::Key(99999), output2));
	ASSERT_EQ(output2, input);
	ASSERT_FALSE(storage.get(omx::Key(100001), output3));
	ASSERT_FALSE(storage.get(omx::Key(10), output3));
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

