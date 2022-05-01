#pragma once

#include "Response.h"
#include "Request.h"
#include "internal/FastPimpl.h"

#include <string>
#include <future>
#include <optional>

namespace omx {

	class DatabaseConnectorImpl;

	/**
	 * @brief Simple asynchronous tcp client for OMXD.
	 */
	class DatabaseConnector {
	public:

		/**
		 *  Constructor initializes the tcp socket and start the i/o service.
		 * @param address remote server IPv4 address.
		 * @param port remote server IPv4 port.
		 */
		DatabaseConnector(std::string address, uint16_t port);

		~DatabaseConnector();

		/**
		 * @brief Send request to the server.
		 * @param request OMXDB request
		 */
		std::future<omx::Response> execute(omx::Request request);

	private:
		static constexpr std::size_t kImplSize = 72;
		static constexpr std::size_t kImplAlign = 8;
		omx::FastPimpl<DatabaseConnectorImpl, kImplSize, kImplAlign> m_impl;
	};

}
