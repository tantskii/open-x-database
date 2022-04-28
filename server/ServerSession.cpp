#include "ServerSession.h"

#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>

#include <iostream>

using namespace boost::asio::ip;

namespace omx {

	ServerSession::ServerSession(DatabasePtr database, SocketPtr socket)
		: m_database(std::move(database))
		, m_socket(std::move(socket))
	{}

	void ServerSession::startHandling() {
		constexpr uint32_t kContentLengthSize = sizeof(ContentLength);

		auto matchCondition = boost::asio::transfer_exactly(kContentLengthSize);

		auto readHandler = [this](const BoostError& errorCode, size_t bytesTransferred) {
			onContentLengthReceived(errorCode, bytesTransferred);
		};

		boost::asio::async_read(*m_socket, m_requestBuffer, matchCondition, readHandler);
	}

	void ServerSession::onRequestReceived(const BoostError& errorCode, size_t bytesTransferred) {

		if (errorCode) {
			std::cerr
				<< " Error code = " << errorCode.value()
				<< ". Message: " << errorCode.message();

			onFinish();

			return;
		}

		std::cout << " Request bytes transferred: " << bytesTransferred;

		m_response = processRequest(m_requestBuffer);

		auto responseSerialized = m_response.serialize();

		auto writeHandler = [this](const BoostError& errorCode, size_t bytesTransferred) {
			onResponseSent(errorCode, bytesTransferred);
		};

		boost::asio::async_write(*m_socket, boost::asio::buffer(responseSerialized), writeHandler);
	}

	void ServerSession::onResponseSent(const BoostError& errorCode, size_t bytesTransferred) {

		if (errorCode) {
			std::cerr
				<< " Error code = " << errorCode.value()
				<< ". Message: " << errorCode.message();
		}

		std::cout << " Response bytes transferred: " << bytesTransferred;

		onFinish();
	}

	void ServerSession::onFinish() {
		delete this;
	}

	omx::Response ServerSession::processRequest(boost::asio::streambuf& requestBuffer) const {
		auto serializedRequest = std::string(
			std::istreambuf_iterator<char>(&requestBuffer),
			std::istreambuf_iterator<char>());

		auto request = omx::Request();
		request.deserialize(serializedRequest);

		std::cout
			<< " Received:"
			<< " Request type = " << static_cast<int>(request.requestType)
			<< " Key = " << request.key.id;

		return m_database->handle(request);
	}

	void ServerSession::onContentLengthReceived(const BoostError& errorCode, size_t numBytes) {
		if (errorCode) {
			std::cerr
				<< " Error code = " << errorCode.value()
				<< ". Message: " << errorCode.message();

			onFinish();

			return;
		}

		std::cout << " Content length bytes transferred: " << numBytes;

		assert(numBytes == sizeof(Request::contentLength));

		m_contentLength = *reinterpret_cast<const ContentLength*>(m_requestBuffer.data().data());

		auto matchCondition = boost::asio::transfer_exactly(m_contentLength);

		auto readHandler = [this](const BoostError& errorCode, size_t bytesTransferred) {
			onRequestReceived(errorCode, bytesTransferred);
		};

		boost::asio::async_read(*m_socket, m_requestBuffer, matchCondition, readHandler);
	}

}
