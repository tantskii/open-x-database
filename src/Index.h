#pragma once

#include <omx/Key.h>

#include <unordered_map>
#include <shared_mutex>
#include <mutex>
#include <iostream>

namespace omx {

	struct SearchHint {
		explicit SearchHint(std::size_t fileId = 0, std::size_t offset = 0, std::size_t size = 0);

		std::size_t fileId;
		std::size_t offset;
		std::size_t size;
	};

	class Index {
	public:

		void insert(Key key, SearchHint hint);

		void merge(const Index& other);

		bool get(Key key, SearchHint& hint) const;

		void dump(std::ostream& stream);

		void load(std::istream& stream);

	private:
		std::unordered_map<Key, SearchHint, KeyHasher> m_map;
	};

}
