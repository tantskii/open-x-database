#pragma once

#include "Aliases.h"

#include <omx/Request.h>

#include <boost/asio.hpp>

#include <thread>

namespace omx {

	/**
	 * @brief Simple asynchronous tcp client for OMXD.
	 */
	class DatabaseConnection {
	public:

		/**
		 *  Constructor initializes the tcp socket and start the i/o service.
		 * @param address remote server IPv4 address.
		 * @param port remote server IPv4 port.
		 */
		DatabaseConnection(std::string address, uint16_t port);

		~DatabaseConnection();

		/**
		 * @brief Send request to the server.
		 * @param request OMXDB request
		 */
		omx::Response execute(const omx::Request& request);

	private:
		const std::string m_address;
		const uint16_t m_port;

		boost::asio::io_service m_service;
		Socket m_socket;

		std::thread m_thread;
	};

}
