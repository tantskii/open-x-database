#pragma once

#include "Aliases.h"

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>

#include <memory>

namespace omx {

	/**
	 * @brief Session class is intended to handle a single connected client by reading the
	 * request, processing it, and then sending back the response message.
	 */
	class Session {
	public:

		explicit Session(DatabasePtr database, SocketPtr socket);

		/**
		 * @brief This method starts handling the client by initiating the asynchronous
		 *  reading operation to read the request message from the client.
		 */
		void startHandling();

	private:

		/**
		 * @brief Callback for receiving request content length
		 * @param errorCode
		 * @param bytesTransferred
		 */
		void onContentLengthReceived(const BoostError& errorCode, std::size_t bytesTransferred);

		/**
		 * @brief Callback for the incoming request.
		 * @param errorCode
		 * @param bytesTransferred
		 */
		void onRequestReceived(const BoostError& errorCode, std::size_t bytesTransferred);

		/**
		 * @brief Callback for the response event.
		 * @param errorCode
		 * @param bytesTransferred
		 */
		void onResponseSent(const BoostError& errorCode, std::size_t bytesTransferred);

		/**
		 * @brief Callback for cleaning up the allocated session object
		 */
		void onFinish();

		/**
		 * @brief Parse and process the incoming request from client.
		 * @param requestBuffer buffer with serialized request
		 * @return
		 */
		omx::Response processRequest(boost::asio::streambuf& requestBuffer) const;

		using ContentLength = decltype(Response::contentLength);

		SocketPtr m_socket;

		DatabasePtr m_database;

		ContentLength m_contentLength = 0;

		// TODO check if not necessary
		omx::Response m_response;
		boost::asio::streambuf m_requestBuffer;
	};

}
