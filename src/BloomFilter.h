#pragma once

#include <omx/Key.h>

#include "Hasher/IHasher.h"
#include <City.h>

#include <vector>

namespace omx {

	template<uint64_t FilterSize, uint8_t NumHashes>
	class BloomFilter {
	public:
		BloomFilter();

		void add(omx::Key key);

		[[nodiscard]] bool probablyContains(omx::Key key) const;

		void dump(std::ofstream& stream) const;

		void load(std::ifstream& stream);

	private:

		[[nodiscard]] uint64_t rehash(uint8_t index, const UInt128& hash) const;

		[[nodiscard]] UInt128 hash(omx::Key key) const;

		std::vector<uint8_t> m_bits;
	};

	template<uint64_t FilterSize, uint8_t NumHashes>
	UInt128 BloomFilter<FilterSize, NumHashes>::hash(omx::Key key) const {
		return CityHash128(reinterpret_cast<const char*>(&key), sizeof(key));
	}

	template<uint64_t FilterSize, uint8_t NumHashes>
	uint64_t BloomFilter<FilterSize, NumHashes>::rehash(const uint8_t index, const UInt128& hash) const {
		return (hash.first + index * hash.second) % m_bits.size();
	}

	template<uint64_t FilterSize, uint8_t NumHashes>
	void BloomFilter<FilterSize, NumHashes>::add(omx::Key key) {
		const auto hashValue = hash(key);

		for (uint8_t i = 0; i < NumHashes; ++i) {
			const auto index = rehash(i, hashValue);
			m_bits[index] = 1;
		}
	}

	template<uint64_t FilterSize, uint8_t NumHashes>
	bool BloomFilter<FilterSize, NumHashes>::probablyContains(omx::Key key) const {
		const auto hashValue = hash(key);

		for (uint8_t i = 0; i < NumHashes; ++i) {
			const auto index = rehash(i, hashValue);

			if (m_bits[index] == 0) {
				return false;
			}
		}

		return true;
	}

	template<uint64_t FilterSize, uint8_t NumHashes>
	BloomFilter<FilterSize, NumHashes>::BloomFilter()
		: m_bits(FilterSize, 0)
	{}

	template<uint64_t FilterSize, uint8_t NumHashes>
	void BloomFilter<FilterSize, NumHashes>::dump(std::ofstream& stream) const {
		if (!stream.is_open()) {
			throw std::runtime_error("invalid output stream");
		}

		stream.write(reinterpret_cast<const char*>(m_bits.data()), FilterSize);
		stream.flush();
	}

	template<uint64_t FilterSize, uint8_t NumHashes>
	void BloomFilter<FilterSize, NumHashes>::load(std::ifstream& stream) {
		if (!stream.is_open()) {
			throw std::runtime_error("invalid input stream");
		}

		stream.read(reinterpret_cast<char*>(m_bits.data()), FilterSize);
		stream.peek();
	}

}
