#pragma once

#include <cstdint>

namespace omx {
	template <typename TKey, typename TKeyComp>
	struct InsertKey {
		InsertKey(uint64_t id_, TKey key_);

		struct Comparator {
			bool operator()(const InsertKey& lhs, const InsertKey& rhs) const;
		};

		uint64_t id;
		TKey key;
	};

	template <typename TKey>
	struct SearchKey {
		SearchKey(TKey key_);

		TKey key;
	};

	template<typename TKey, typename TKeyComp>
	InsertKey<TKey, TKeyComp>::InsertKey(uint64_t id_, TKey key_)
		: id(id_), key(key_)
	{}

	template<typename TKey, typename TKeyComp>
	bool InsertKey<TKey, TKeyComp>::Comparator::operator()(
		const InsertKey& lhs,
		const InsertKey& rhs) const
	{
		TKeyComp comp;
		if (comp(lhs.key, rhs.key)) {
			return true;
		}
		if (comp(rhs.key, lhs.key)) {
			return false;
		}
		return lhs.id > rhs.id;
	}

	template<typename TKey>
	SearchKey<TKey>::SearchKey(TKey key_)
		: key(key_)
	{}

	template<typename TKey, typename TKeyComp>
	bool operator<(const InsertKey<TKey, TKeyComp>& ik, const SearchKey<TKey>& sk) {
		TKeyComp comp;
		return comp(ik.key, sk.key);
	}

	template<typename TKey, typename TKeyComp>
	bool operator<(const SearchKey<TKey>& sk, const InsertKey<TKey, TKeyComp>& ik) {
		TKeyComp comp;
		return comp(sk.key, ik.key);
	}

	template<typename TKey, typename TKeyComp>
	bool operator<(const InsertKey<TKey, TKeyComp>& lhs, const InsertKey<TKey, TKeyComp>& rhs) {
		typename InsertKey<TKey, TKeyComp>::Comparator comp;
		return comp(lhs, rhs);
	}
}
