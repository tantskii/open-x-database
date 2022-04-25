#pragma once

#include "Aliases.h"

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <atomic>
#include <memory>

namespace omx {

	/**
	 * @brief The Acceptor accepts a port number on which it will listen
	 * for the incoming connection requests.
	 */
	class Acceptor {
	public:

		/**
		 * @brief acceptor constructor
		 * @param service instance of 'io_service' class is required by socket constructor.
		 * @param port port number on which it will listen for the incoming connection
		 */
		Acceptor(DatabasePtr database, boost::asio::io_service& service, uint16_t port);

		/**
		 * @brief Start listening and accept incoming connection requests
		 */
		void start();

		/**
		 * @brief Stop listening and accept incoming connection requests
		 */
		void stop();

		DatabasePtr getDatabasePtr();

	private:

		/**
		 * @brief Constructs a socket object and initiates the asynchronous accept operation
		 */
		void initAccept();

		/**
		 * @brief Callback which starts handling the connected client.
		 * @param errorCode error code
		 * @param socket instance of 'socket' class
		 */
		void onAccept(const BoostError& errorCode, Socket socket);

		boost::asio::io_service& m_service;
		boost::asio::ip::tcp::acceptor m_acceptor;
		std::atomic<bool> m_isStopped;

		DatabasePtr m_database;
	};

}
