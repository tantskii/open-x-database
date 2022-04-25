#include "ServerBase.h"

namespace omx {

	ServerBase::ServerBase(uint32_t numThreads)
		: m_numThreads(numThreads)
	{}

	void ServerBase::start() {
		if (init()) {
			m_threads.resize(m_numThreads);
			for (auto& thread: m_threads) {
				thread = std::thread([this]() { m_service.run(); });
			}
		}
	}

	void ServerBase::stop() {
		if (close()) {
			m_service.stop();

			for (auto& thread: m_threads) {
				thread.join();
			}
		}
	}

}
