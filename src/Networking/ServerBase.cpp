#include "ServerBase.h"

namespace omx {

	ServerBase::ServerBase(uint32_t numThreads)
		: m_numThreads(numThreads), m_work(m_service)
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

	void ServerBase::wait() {
		m_service.run();
	}

}
