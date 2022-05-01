#pragma once

#include "Acceptor.h"
#include "ServerBase.h"

namespace omx {

	class DatabaseServerImpl : public ServerBase {
	public:
		DatabaseServerImpl(uint16_t port, uint32_t numThreads, std::string databaseDir, omx::Options = {});
		~DatabaseServerImpl() override;
	private:

		bool init() override;

		bool close() override;

		uint16_t m_port;
		Acceptor m_acceptor;

		const std::string m_databaseDir;
		const omx::Options m_databaseOptions;
	};

}
