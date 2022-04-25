#pragma once

#include <omx/Database.h>

#include <boost/asio/io_service.hpp>

#include <cstdint>
#include <thread>
#include <vector>
#include <memory>

namespace omx {

	class ServerBase {
	public:
		explicit ServerBase(uint32_t numThreads);

		virtual ~ServerBase() = default;

		void start();

		void stop();

	protected:

		virtual bool init() = 0;

		virtual bool close() = 0;

		uint32_t m_numThreads;
		std::vector<std::thread> m_threads;
		boost::asio::io_service m_service = {};
	};

}
