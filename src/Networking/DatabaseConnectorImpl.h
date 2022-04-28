#pragma once

#include "Aliases.h"
#include "ClientSession.h"

#include <omx/Request.h>

#include <boost/asio.hpp>

#include <thread>

namespace omx {

	/**
	 * @brief Simple asynchronous tcp client for OMXD.
	 */
	class DatabaseConnectorImpl {
	public:

		/**
		 *  Constructor initializes the tcp socket and start the i/o service.
		 * @param address remote server IPv4 address.
		 * @param port remote server IPv4 port.
		 */
		DatabaseConnectorImpl(std::string address, uint16_t port);

		~DatabaseConnectorImpl();

		/**
		 * @brief Send request to the server.
		 * @param request OMXDB request
		 */
		std::future<omx::Response> execute(const omx::Request& request);

	private:
		const std::string m_address;
		const uint16_t m_port;

		boost::asio::io_service m_service;
		boost::asio::io_service::work m_work;

		std::thread m_thread;
	};

}
