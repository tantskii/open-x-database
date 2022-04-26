#pragma once

#include "Acceptor.h"
#include "ServerBase.h"

namespace omx {

	class Server : public ServerBase {
	public:
		Server(uint16_t port, uint32_t numThreads, std::string databaseDir, omx::Options = {});
		~Server() override;
	private:

		bool init() override;

		bool close() override;

		uint16_t m_port;
		Acceptor m_acceptor;

		const std::string m_databaseDir;
		const omx::Options m_databaseOptions;
	};

}
