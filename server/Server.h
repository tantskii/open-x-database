#pragma once

#include "Acceptor.h"
#include "ServerBase.h"

namespace omx {

	class Server : public ServerBase {
	public:
		Server(uint16_t port, uint32_t numThreads);

	private:

		bool init() override;

		bool close() override;

		uint16_t m_port;
		Acceptor m_acceptor;
	};

}
