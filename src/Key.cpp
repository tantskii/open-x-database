#include <omx/Key.h>

#include <functional>

namespace omx {

	Key::Key(uint64_t id_)
		: id(id_)
	{}

	bool Key::operator==(const Key& other) const {
		return id == other.id;
	}

	bool Key::operator<(const Key& other) const {
		return id < other.id;
	}

	std::size_t KeyHasher::operator()(const Key& key) const {
		std::hash<std::size_t> hasher;
		return hasher(key.id);
	}

} // namespace omx