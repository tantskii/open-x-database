#include "../include/omx/Key.h"

#include <leveldb/slice.h>

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

} // namespace omx