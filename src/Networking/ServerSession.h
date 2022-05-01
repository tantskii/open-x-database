#pragma once

#include "Aliases.h"
#include "UUID.h"

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>

#include <memory>

namespace omx {

	/**
	 * @brief Session class is intended to handle a single connected client by reading the
	 * request, processing it, and then sending back the response message.
	 */
	class ServerSession : public std::enable_shared_from_this<ServerSession> {
	public:

		explicit ServerSession(DatabasePtr database, SocketPtr socket);

		/**
		 * @brief This method starts handling the client by initiating the asynchronous
		 *  reading operation to read the request message from the client.
		 */
		void run();

	private:

		using Ptr = std::shared_ptr<ServerSession>;

		/**
		 * @brief Callback for receiving request content length
		 * @param errorCode
		 * @param bytesTransferred
		 */
		void onContentLengthReceived(Ptr self, const BoostError& errorCode, size_t numBytes);

		/**
		 * @brief Callback for the incoming request.
		 * @param errorCode
		 * @param bytesTransferred
		 */
		void onRequestReceived(Ptr self, const BoostError& errorCode, size_t numBytes);

		/**
		 * @brief Callback for the response event.
		 * @param errorCode
		 * @param bytesTransferred
		 */
		void onResponseSent(Ptr self, const BoostError& errorCode, size_t numBytes);

		void onError(const BoostError& errorCode);

		/**
		 * @brief Parse and process the incoming request from client.
		 * @param requestBuffer buffer with serialized request
		 * @return
		 */
		omx::Response processRequest(boost::asio::streambuf& requestBuffer) const;

	private:
		using ContentLength = decltype(Response::contentLength);

		SocketPtr m_socket;

		DatabasePtr m_database;

		ContentLength m_contentLength = 0;

		boost::asio::streambuf m_requestBuffer;

		UUID m_tag;
	};

}
