#include <omx/Database.h>
#include <omx/DatabaseServer.h>
#include <omx/DatabaseConnector.h>

#include "../src/MemTable.h"
#include "../src/File.h"
#include "../src/SSTable.h"
#include "../src/SSTableRow.h"
#include "../src/StorageEngine.h"
#include "../src/SSTableIndex.h"
#include "../src/BloomFilter.h"

#include <gtest/gtest.h>

#include <vector>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <thread>

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

	auto input = std::make_shared<omx::SSTableRow>(key, value);
	std::string data = omx::serialize(input);
	auto output = omx::deserialize(data);

	ASSERT_EQ(input->getOperationType(), output->getOperationType());
	ASSERT_EQ(input->getKey().id, output->getKey().id);
	ASSERT_EQ(input->getData(), output->getData());
}

TEST(File, ReadWrite) {
	CLEAR_DIR(temp_dir);

	auto file = omx::File(temp_dir / "file.bin");

	file.append("0123456789");

	{
		std::string str;

		file.read(0, 3, str);

		ASSERT_EQ(str, "012");
	}

	{
		std::string str;

		file.read(4, 3, str);

		ASSERT_EQ(str, "456");
	}

	{
		std::string str;

		auto file2 = omx::File(temp_dir / "file2.bin");

		ASSERT_THROW(file2.read(4, 3, str), std::runtime_error);
	}

	{
		std::string str;

		ASSERT_THROW(file.read(4, 100, str), std::runtime_error);
	}

	{
		std::string str;

		file.write(2, "xxx");

		file.read(0, 10, str);

		ASSERT_EQ(str, "01xxx56789");
	}

	{
		std::string str;

		file.write(8, "abc");

		ASSERT_THROW(file.read(0, 12, str), std::runtime_error);

		file.read(0, 11, str);

		ASSERT_EQ(str, "01xxx567abc");
	}

}

TEST(BloomFilter, ReadWrite) {
	constexpr uint64_t kFilterSize = 10'000'000;
	constexpr uint8_t kNumHashes = 7;

	auto bloomFilter = omx::BloomFilter<kFilterSize, kNumHashes>{};

	for (uint64_t i = 0; i < 100'000; ++i) {
		if (i % 100 != 0) {
			bloomFilter.add(omx::Key(i));
		}
	}

	for (uint64_t i = 0; i < 100'000; ++i) {
		bool result = bloomFilter.probablyContains(omx::Key(i));

		if (i % 100 != 0) {
			ASSERT_TRUE(result);
		} else {
			ASSERT_FALSE(result);
		}
	}
}

TEST(Request, Serialization) {
	auto request0 = omx::Request{omx::RequestType::Put, omx::Key(123), "test string"};
	auto request1 = omx::Request{};

	const auto buffer = request0.serialize();
	request1.deserialize(buffer);

	ASSERT_EQ(request0, request1);
}

TEST(Response, Serialization) {
	auto response0 = omx::Response{omx::ResponseStatus::NotFound, "test string"};
	auto response1 = omx::Response{};

	const auto buffer = response0.serialize();
	response1.deserialize(buffer);

	ASSERT_EQ(response0, response1);
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

TEST(Index, Update) {
	auto index_1 = std::make_unique<omx::SSTableIndex>(1);
	auto index_2 = std::make_unique<omx::SSTableIndex>(2);
	auto index   = std::make_unique<omx::Index>();
	omx::SearchHint hint;

	index_1->insert(omx::Key(1), omx::FileSearchHint(100, 10));
	index_1->insert(omx::Key(2), omx::FileSearchHint(200, 20));
	index_1->insert(omx::Key(3), omx::FileSearchHint(300, 30));
	index_2->insert(omx::Key(3), omx::FileSearchHint(400, 40));
	index_2->insert(omx::Key(4), omx::FileSearchHint(500, 50));

	index->update(std::move(index_1));
	index->update(std::move(index_2));

	ASSERT_TRUE(index->get(omx::Key(1), hint));
	ASSERT_EQ(hint.fileId, 1);
	ASSERT_EQ(hint.offset, 100);
	ASSERT_EQ(hint.size, 10);

	ASSERT_TRUE(index->get(omx::Key(2), hint));
	ASSERT_EQ(hint.fileId, 1);
	ASSERT_EQ(hint.offset, 200);
	ASSERT_EQ(hint.size, 20);

	ASSERT_TRUE(index->get(omx::Key(3), hint));
	ASSERT_EQ(hint.fileId, 2);
	ASSERT_EQ(hint.offset, 400);
	ASSERT_EQ(hint.size, 40);

	ASSERT_TRUE(index->get(omx::Key(4), hint));
	ASSERT_EQ(hint.fileId, 2);
	ASSERT_EQ(hint.offset, 500);
	ASSERT_EQ(hint.size, 50);

	ASSERT_FALSE(index->get(omx::Key(5), hint));
	ASSERT_EQ(hint.fileId, 0);
	ASSERT_EQ(hint.offset, 0);
	ASSERT_EQ(hint.size, 0);
}

TEST(Index, DumpRestore) {
	CLEAR_DIR(temp_dir);

	auto index_1 = std::make_unique<omx::SSTableIndex>(1);
	auto index_2 = std::make_unique<omx::SSTableIndex>(2);
	auto index   = std::make_unique<omx::Index>();
	omx::SearchHint hint;

	index_1->insert(omx::Key(1), omx::FileSearchHint(100, 10));
	index_1->insert(omx::Key(2), omx::FileSearchHint(200, 20));
	index_1->insert(omx::Key(3), omx::FileSearchHint(300, 30));
	index_2->insert(omx::Key(3), omx::FileSearchHint(400, 40));
	index_2->insert(omx::Key(4), omx::FileSearchHint(500, 50));

	{
		std::ofstream file(temp_dir / "index_1.bin", std::ios::binary);
		index_1->dump(file);
	}

	{
		std::ofstream file(temp_dir / "index_2.bin", std::ios::binary);
		index_2->dump(file);
	}

	index->load(temp_dir);

	ASSERT_TRUE(index->get(omx::Key(1), hint));
	ASSERT_EQ(hint.fileId, 1);
	ASSERT_EQ(hint.offset, 100);
	ASSERT_EQ(hint.size, 10);

	ASSERT_TRUE(index->get(omx::Key(2), hint));
	ASSERT_EQ(hint.fileId, 1);
	ASSERT_EQ(hint.offset, 200);
	ASSERT_EQ(hint.size, 20);

	ASSERT_TRUE(index->get(omx::Key(3), hint));
	ASSERT_EQ(hint.fileId, 2);
	ASSERT_EQ(hint.offset, 400);
	ASSERT_EQ(hint.size, 40);

	ASSERT_TRUE(index->get(omx::Key(4), hint));
	ASSERT_EQ(hint.fileId, 2);
	ASSERT_EQ(hint.offset, 500);
	ASSERT_EQ(hint.size, 50);

	ASSERT_FALSE(index->get(omx::Key(5), hint));
	ASSERT_EQ(hint.fileId, 0);
	ASSERT_EQ(hint.offset, 0);
	ASSERT_EQ(hint.size, 0);
}

TEST(SSTableIndex, ReadWrite) {
	const uint32_t fileId = 1;
	omx::SSTableIndex index(fileId);
	omx::FileSearchHint hint;

	index.insert(omx::Key(1), omx::FileSearchHint(100, 40));
	index.insert(omx::Key(2), omx::FileSearchHint(200, 30));
	index.insert(omx::Key(3), omx::FileSearchHint(300, 20));
	index.insert(omx::Key(3), omx::FileSearchHint(400, 10));

	ASSERT_EQ(index.getFileId(), fileId);

	ASSERT_TRUE(index.get(omx::Key(1), hint));
	ASSERT_EQ(hint.size, 40);
	ASSERT_EQ(hint.offset, 100);

	ASSERT_TRUE(index.get(omx::Key(2), hint));
	ASSERT_EQ(hint.size, 30);
	ASSERT_EQ(hint.offset, 200);

	ASSERT_TRUE(index.get(omx::Key(3), hint));
	ASSERT_EQ(hint.size, 10);
	ASSERT_EQ(hint.offset, 400);

	ASSERT_FALSE(index.get(omx::Key(4), hint));
	ASSERT_EQ(hint.size, 0);
	ASSERT_EQ(hint.offset, 0);
}

TEST(SSTableIndex, DumpRestore) {
	CLEAR_DIR(temp_dir);

	const uint32_t fileId = 123;
	omx::SSTableIndex index_0(fileId);
	omx::SSTableIndex index_1;
	omx::FileSearchHint hint;

	index_0.insert(omx::Key(1), omx::FileSearchHint(100, 40));
	index_0.insert(omx::Key(2), omx::FileSearchHint(200, 30));
	index_0.insert(omx::Key(3), omx::FileSearchHint(300, 20));
	index_0.insert(omx::Key(3), omx::FileSearchHint(400, 10));

	{
		std::ofstream file(temp_dir / "index.bin", std::ios::binary | std::ios::app);
		index_0.dump(file);
	}

	{
		std::ofstream file(temp_dir / "dir/index.bin", std::ios::binary | std::ios::app);
		ASSERT_THROW(index_0.dump(file), std::runtime_error);
	}

	{
		std::ifstream file(temp_dir / "index.bin", std::ios::binary | std::ios::in);
		index_1.load(file);
	}

	{
		std::ifstream file(temp_dir / "index1.bin", std::ios::binary | std::ios::in);
		ASSERT_THROW(index_1.load(file), std::runtime_error);
	}

	ASSERT_EQ(index_1.getFileId(), fileId);

	ASSERT_TRUE(index_1.get(omx::Key(1), hint));
	ASSERT_EQ(hint.size, 40);
	ASSERT_EQ(hint.offset, 100);

	ASSERT_TRUE(index_1.get(omx::Key(2), hint));
	ASSERT_EQ(hint.size, 30);
	ASSERT_EQ(hint.offset, 200);

	ASSERT_TRUE(index_1.get(omx::Key(3), hint));
	ASSERT_EQ(hint.size, 10);
	ASSERT_EQ(hint.offset, 400);

	ASSERT_FALSE(index_1.get(omx::Key(4), hint));
	ASSERT_EQ(hint.size, 0);
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
	ASSERT_EQ(rows[0]->getData(), "");
	ASSERT_EQ(rows[1]->getKey(), omx::Key(1));
	ASSERT_EQ(rows[1]->getOperationType(), omx::EntryType::Put);
	ASSERT_EQ(rows[1]->getData(), "0020");
}

TEST(SSTable, LoadDump) {
	CLEAR_DIR(temp_dir);

	std::string filename = temp_dir / "sstable.bin";

	omx::SSTable lhs;
	omx::SSTable rhs;

	lhs.append(std::make_shared<omx::SSTableRow>(omx::Key(1)));
	lhs.append(std::make_shared<omx::SSTableRow>(omx::Key(2), "0002"));
	lhs.append(std::make_shared<omx::SSTableRow>(omx::Key(3), "0003"));

	{
		std::ofstream stream(filename, std::ios::binary | std::ios::out);
		lhs.dump(stream);
	}

	{
		std::ifstream stream(filename, std::ios::binary | std::ios::in);
		rhs.load(stream);
	}

	const auto& l = lhs.getRowList();
	const auto& r = rhs.getRowList();
	ASSERT_EQ(l.size(), r.size());
	for (int i = 0; i < 3; ++i) {
		ASSERT_EQ(l[i]->getKey(), r[i]->getKey());
		ASSERT_EQ(l[i]->getOperationType(), r[i]->getOperationType());
		ASSERT_EQ(l[i]->getData(), r[i]->getData());
	}
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
	omx::SSTableRowPtr entry;
	omx::FileSearchHint hint;
	std::string data;
	std::string buffer;

	table.put(omx::Key(1), "111111111111111111111111111111111111111111111111111111");
	table.put(omx::Key(2), "222222222222222222222222222222222222222222222222222222");
	table.put(omx::Key(3), "333333333333333333333333333333333333333333333333333333");
	table.remove(omx::Key(1));

	std::ostringstream os;

	auto index = table.createSortedStringsTableIndex(0);

	table.dump(os);

	data = os.str();

	ASSERT_TRUE(index->get(omx::Key(2), hint));
	buffer = data.substr(hint.offset, hint.size);
	entry = omx::deserialize(buffer);

	ASSERT_EQ(entry->getOperationType(), omx::EntryType::Put);
	ASSERT_EQ(entry->getData(), "222222222222222222222222222222222222222222222222222222");
	ASSERT_EQ(entry->getKey(), omx::Key(2));

	ASSERT_TRUE(index->get(omx::Key(1), hint));
	buffer = data.substr(hint.offset, hint.size);
	entry = omx::deserialize(buffer);

	ASSERT_EQ(entry->getOperationType(), omx::EntryType::Remove);
	ASSERT_TRUE(entry->getData().empty());
	ASSERT_EQ(entry->getKey(), omx::Key(1));
}

TEST(MemTable, Restore) {
	CLEAR_DIR(temp_dir);

	std::string filename = temp_dir / "log.bin";
	std::string value;

	{
		omx::MemTable table;
		table.setWriteAheadLog(filename);
		table.put(omx::Key(1), "111111111111111111111111111111111111111111111111111111");
		table.put(omx::Key(2), "222222222222222222222222222222222222222222222222222222");
		table.put(omx::Key(3), "333333333333333333333333333333333333333333333333333333");
		table.remove(omx::Key(1));
	}

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

	omx::StorageEngine storage;
	storage.open(temp_dir);

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

TEST(StorageEngine, Handle) {

	CLEAR_DIR(temp_dir);

	omx::StorageEngine storage;
	storage.open(temp_dir);

	std::string input = "111111111111111111111111111111111111111111111111111111";

	auto response = omx::Response{};

	for (int i = 1; i <= 100000; ++i) {
		response = storage.handle(omx::Request(omx::RequestType::Put, omx::Key(i), input));
		ASSERT_EQ(response.responseStatus, omx::ResponseStatus::Ok);
		ASSERT_EQ(response.value, "");

		if (i == 10000) {
			response = storage.handle(omx::Request(omx::RequestType::Delete, omx::Key(10)));
			ASSERT_EQ(response.responseStatus, omx::ResponseStatus::Ok);
			ASSERT_EQ(response.value, "");
		}
	}

	response = storage.handle(omx::Request(omx::RequestType::Get, omx::Key(3)));
	ASSERT_EQ(response.responseStatus, omx::ResponseStatus::Ok);
	ASSERT_EQ(response.value, input);

	response = storage.handle(omx::Request(omx::RequestType::Get, omx::Key(99999)));
	ASSERT_EQ(response.responseStatus, omx::ResponseStatus::Ok);
	ASSERT_EQ(response.value, input);

	response = storage.handle(omx::Request(omx::RequestType::Get, omx::Key(100001)));
	ASSERT_EQ(response.responseStatus, omx::ResponseStatus::NotFound);
	ASSERT_EQ(response.value, "");

	response = storage.handle(omx::Request(omx::RequestType::Get, omx::Key(10)));
	ASSERT_EQ(response.responseStatus, omx::ResponseStatus::NotFound);
	ASSERT_EQ(response.value, "");
}

TEST(StorageEngine, Restore) {
	CLEAR_DIR(temp_dir);

	std::string input = "111111111111111111111111111111111111111111111111111111";
	std::string output1;
	std::string output2;
	std::string output3;

	{
		omx::StorageEngine storage;
		storage.open(temp_dir);

		for (int i = 1; i <= 100000; ++i) {
			storage.put(omx::Key(i), input);

			if (i == 10000) {
				storage.remove(omx::Key(10));
			}
		}
	}

	omx::StorageEngine storage;
	storage.open(temp_dir);

	ASSERT_TRUE(storage.get(omx::Key(3), output1));
	ASSERT_EQ(output1, input);
	ASSERT_TRUE(storage.get(omx::Key(99999), output2));
	ASSERT_EQ(output2, input);
	ASSERT_FALSE(storage.get(omx::Key(100001), output3));
	ASSERT_FALSE(storage.get(omx::Key(10), output3));
}

TEST(Database, ReadWriteThreaded) {
	CLEAR_DIR(temp_dir);

	const size_t numReadThreads = 8;
	const size_t numWriteThreads = 2;
	const size_t numInserts = 100'000;
	const size_t numReads = 100'000;
	const size_t valueLength = 100;

	omx::Database database;
	database.open(temp_dir.c_str());

	std::vector<std::thread> readThreads(numReadThreads);
	std::vector<std::thread> writeThreads(numWriteThreads);

	for (int i = 1; i <= writeThreads.size(); ++i) {
		writeThreads[i - 1] = std::thread([&](size_t index) {
			for (size_t j = index; j <= numInserts; j += numWriteThreads) {
				std::string value(valueLength, char(j % 10));
				database.put(omx::Key(j), value);

				if (j == 10000) {
					database.remove(omx::Key(10));
				}
			}
		}, i);
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	for (int i = 1; i <= readThreads.size(); ++i) {
		readThreads[i - 1] = std::thread([&](size_t index) {
			std::string value;

			for (size_t j = index; j <= numReads; j += numReadThreads) {
				bool status = database.get(omx::Key(j), value);

				if (status) {
					std::string reference(valueLength, char(j % 10));
					ASSERT_EQ(value, reference);
				}
			}
		}, i);
	}

	for (auto& thread: writeThreads) {
		thread.join();
	}

	for (auto& thread: readThreads) {
		thread.join();
	}

	std::string out1;
	std::string inp1(valueLength, char(3 % 10));
	ASSERT_TRUE(database.get(omx::Key(3), out1));
	ASSERT_EQ(out1, inp1);

	std::string out2;
	std::string inp2(valueLength, char(99'999 % 10));
	ASSERT_TRUE(database.get(omx::Key(99'999), out2));
	ASSERT_EQ(out2, inp2);

	ASSERT_FALSE(database.get(omx::Key(100'001), out2));
	ASSERT_FALSE(database.get(omx::Key(10), out2));
}

TEST(Networking, Handle) {
	CLEAR_DIR(temp_dir);

	const uint16_t port = 3232;
	const std::string address = "127.0.0.1";
	const std::string databaseDir = temp_dir;
	const uint16_t numThreads = 4;

	auto server    = std::make_shared<omx::DatabaseServer>(port, numThreads, databaseDir);
	auto connector = std::make_shared<omx::DatabaseConnector>(address, port);

	server->start();

	using FutureResponse = std::future<omx::Response>;

	std::vector<FutureResponse> insertFutures(4);
	insertFutures[0] = connector->execute(omx::Request(omx::RequestType::Put, omx::Key(0), "0000"));
	insertFutures[1] = connector->execute(omx::Request(omx::RequestType::Put, omx::Key(1), "1111"));
	insertFutures[2] = connector->execute(omx::Request(omx::RequestType::Put, omx::Key(2), "2222"));
	insertFutures[3] = connector->execute(omx::Request(omx::RequestType::Put, omx::Key(3), "3333"));

	for (FutureResponse& f: insertFutures) {
		auto response = f.get();
		ASSERT_EQ(response.responseStatus, omx::ResponseStatus::Ok);
	}

	std::vector<FutureResponse> receiveFutures(4);
	receiveFutures[0] = connector->execute(omx::Request(omx::RequestType::Get, omx::Key(0)));
	receiveFutures[1] = connector->execute(omx::Request(omx::RequestType::Get, omx::Key(1)));
	receiveFutures[2] = connector->execute(omx::Request(omx::RequestType::Get, omx::Key(2)));
	receiveFutures[3] = connector->execute(omx::Request(omx::RequestType::Get, omx::Key(3)));

	std::vector<omx::Response> responses(4);

	for (int i = 0; i < responses.size(); ++i) {
		responses[i] = receiveFutures[i].get();
		ASSERT_EQ(responses[i].responseStatus, omx::ResponseStatus::Ok);
	}
	ASSERT_EQ(responses[0].value, "0000");
	ASSERT_EQ(responses[1].value, "1111");
	ASSERT_EQ(responses[2].value, "2222");
	ASSERT_EQ(responses[3].value, "3333");

	auto notFoundFuture = connector->execute(omx::Request(omx::RequestType::Get, omx::Key(4)));
	auto removeFuture   = connector->execute(omx::Request(omx::RequestType::Delete, omx::Key(2)));

	{
		auto response = notFoundFuture.get();
		ASSERT_EQ(response.responseStatus, omx::ResponseStatus::NotFound);
	}

	{
		auto response = removeFuture.get();
		ASSERT_EQ(response.responseStatus, omx::ResponseStatus::Ok);
	}

	auto removeFuture2 = connector->execute(omx::Request(omx::RequestType::Get, omx::Key(2)));
	{
		auto response = removeFuture2.get();
		ASSERT_EQ(response.responseStatus, omx::ResponseStatus::NotFound);
	}
}
