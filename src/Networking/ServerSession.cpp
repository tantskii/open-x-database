#include "ServerSession.h"

#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>

#include <iostream>

using namespace boost::asio::ip;

namespace omx {

	ServerSession::ServerSession(DatabasePtr database, SocketPtr socket)
		: m_database(std::move(database))
		, m_socket(std::move(socket))
		, m_tag()
	{
		std::cout << m_tag
			<< " Start new server session."
			<< std::endl;
	}

	ServerSession::~ServerSession() {
		std::cout << m_tag
			<< " Finish server session."
			<< std::endl;
	}

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

		auto response = processRequest(m_requestBuffer);

		auto responseSerialized = response.serialize();

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
	}

	omx::Response ServerSession::processRequest(boost::asio::streambuf& requestBuffer) const {
		auto serializedRequest = std::string(
			std::istreambuf_iterator<char>(&requestBuffer),
			std::istreambuf_iterator<char>());

		auto request = omx::Request();
		request.deserialize(serializedRequest);

		std::cout
			<< m_tag
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

		assert(numBytes == sizeof(ContentLength));

		m_contentLength = *reinterpret_cast<const ContentLength*>(m_requestBuffer.data().data());

		auto matchCondition = boost::asio::transfer_exactly(m_contentLength);

		auto readHandler = [self](const BoostError& error, size_t numBytes) {
			self->onRequestReceived(self, error, numBytes);
		};

		boost::asio::async_read(*m_socket, m_requestBuffer, matchCondition, readHandler);
	}

	void ServerSession::onError(const BoostError& error) {
		std::cerr
			<< m_tag
			<< " Error code = " << error.value()
			<< ". Message: " << error.message();
	}

}
