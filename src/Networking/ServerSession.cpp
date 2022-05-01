#include "ServerSession.h"

#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>

#include <iostream>

using namespace boost::asio::ip;

static void onError(const omx::BoostError& error) {
	std::cerr
		<< " Error code = " << error.value()
		<< ". Message: " << error.message();
}

namespace omx {

	ServerSession::ServerSession(DatabasePtr database, SocketPtr socket)
		: m_database(std::move(database))
		, m_socket(std::move(socket))
	{}

	void ServerSession::run() {
		auto matchCondition = boost::asio::transfer_exactly(sizeof(ContentLength));

		auto self = shared_from_this();

		auto readHandler = [self](const BoostError& error, const size_t numBytes) {
			self->onContentLengthReceived(self, error, numBytes);
		};

		boost::asio::async_read(*m_socket, m_requestBuffer, matchCondition, readHandler);
	}

	void ServerSession::onRequestReceived(Ptr self, const BoostError& error, const size_t numBytes) {
		if (error) {
			onError(error);
			return;
		}

		std::cout << " Request bytes transferred: " << numBytes << std::endl;

		m_response = processRequest(m_requestBuffer);

		auto responseSerialized = m_response.serialize();

		auto writeHandler = [self](const BoostError& error, const size_t numBytes) {
			self->onResponseSent(self, error, numBytes);
		};

		boost::asio::async_write(*m_socket, boost::asio::buffer(responseSerialized), writeHandler);
	}

	void ServerSession::onResponseSent(Ptr self, const BoostError& error, const size_t numBytes) {
		if (error) {
			onError(error);
			return;
		}

		std::cout << " Response bytes transferred: " << numBytes << std::endl;
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
			<< " Key = " << request.key.id
			<< std::endl;

		return m_database->handle(request);
	}

	void ServerSession::onContentLengthReceived(Ptr self, const BoostError& error, const size_t numBytes) {
		if (error) {
			onError(error);
			return;
		}

		std::cout << " Content length bytes transferred: " << numBytes << std::endl;

		assert(numBytes == sizeof(ContentLength));

		m_contentLength = *reinterpret_cast<const ContentLength*>(m_requestBuffer.data().data());

		auto matchCondition = boost::asio::transfer_exactly(m_contentLength);

		auto readHandler = [self](const BoostError& error, size_t numBytes) {
			self->onRequestReceived(self, error, numBytes);
		};

		boost::asio::async_read(*m_socket, m_requestBuffer, matchCondition, readHandler);
	}

}
