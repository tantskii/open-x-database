#pragma once

#include "Options.h"
#include "internal/FastPimpl.h"

#include <string>

namespace omx {

	class DatabaseServerImpl;

	class DatabaseServer {
	public:

		DatabaseServer(uint16_t port, uint32_t numThreads, std::string databaseDir, omx::Options = {});

		~DatabaseServer();

		void start();

		void stop();

		void wait();

	private:
		static constexpr std::size_t kImplSize = 232;
		static constexpr std::size_t kImplAlign = 8;
		omx::FastPimpl<DatabaseServerImpl, kImplSize, kImplAlign> m_impl;
	};
}
