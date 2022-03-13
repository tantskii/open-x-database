#include "../include/omx/Key.h"

#include <leveldb/slice.h>

namespace omx {

	Key::Key(uint64_t id_)
		: id(id_)
	{}

} // namespace omx