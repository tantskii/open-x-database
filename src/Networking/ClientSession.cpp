#include "ClientSession.h"

#include <iostream>

#define HANDLE_BOOST_ERROR(__error)        \
{                                          \
	if (__error) {                         \
		self->onError(__error);            \
		return;                            \
	}                                      \
}

namespace omx {

	ClientSession::ClientSession(
		SocketPtr socket,
		ResolverPtr resolver,
		QueryPtr query,
		Request request)
		: m_socket(std::move(socket))
		, m_resolver(std::move(resolver))
		, m_query(std::move(query))
		, m_request(std::move(request))
		, m_tag()
	{
		std::cout << m_tag
			<< " Start new client session:"
			<< " Request type = " << static_cast<int>(m_request.requestType)
			<< " Key id = " << m_request.key.id
			<< std::endl;
	}

	ClientSession::~ClientSession() {
		std::cout << m_tag
			<< " Finish client session"
			<< std::endl;
	}

	std::future<omx::Response> ClientSession::run() {
		auto future = m_promise.get_future();

		try {
			auto self = shared_from_this();

			auto resolveHandler = [self](const BoostError& error, const Resolver::iterator& it) {
				self->onResolve(self, error, it);
			};

			m_resolver->async_resolve(*m_query, resolveHandler);
		} catch (const std::exception& e) {
			m_promise.set_exception(std::current_exception());
		}

		return future;
	}

	void ClientSession::onResolve(Ptr self, const BoostError& error, const Resolver::iterator& it) {
		HANDLE_BOOST_ERROR(error);

		auto connectHandler = [self](const BoostError& error) {
			self->onConnect(self, error);
		};

		m_socket->async_connect(it->endpoint(), connectHandler);
	}

	void ClientSession::onConnect(Ptr self, const BoostError& error) {
		HANDLE_BOOST_ERROR(error);

		auto sendHandler = [self](const BoostError& error, size_t numBytes) {
			self->onSend(self, error, numBytes);
		};

		m_socket->async_send(boost::asio::buffer(m_request.serialize()), sendHandler);
	}

	void ClientSession::onSend(Ptr self, const BoostError& error, const size_t numBytes) {
		HANDLE_BOOST_ERROR(error);

		auto readHandler = [self](const BoostError& error, const size_t numBytes) {
			self->onContentLengthReceive(self, error, numBytes);
		};

		auto matchCondition = boost::asio::transfer_exactly(sizeof(ContentLength));

		boost::asio::async_read(*m_socket, m_buffer, matchCondition, readHandler);
	}

	void ClientSession::onContentLengthReceive(Ptr self, const BoostError& error, size_t numBytes) {
		HANDLE_BOOST_ERROR(error);

		assert(numBytes == sizeof(ContentLength));

		m_contentLength = *reinterpret_cast<const ContentLength*>(m_buffer.data().data());

		auto matchCondition = boost::asio::transfer_exactly(m_contentLength);

		auto receiveHandler = [self](const BoostError& error, size_t numBytes) {
			self->onReceive(self, error, numBytes);
		};

		boost::asio::async_read(*m_socket, m_buffer, matchCondition, receiveHandler);
	}

	void ClientSession::onReceive(Ptr self, const BoostError& error, size_t numBytes) {
		HANDLE_BOOST_ERROR(error);

		assert(numBytes == m_contentLength);

		const auto responseSerialized = std::string(
			std::istreambuf_iterator<char>(&m_buffer),
			std::istreambuf_iterator<char>());

		auto response = omx::Response();
		response.deserialize(responseSerialized);

		m_promise.set_value(std::move(response));
	}

	void ClientSession::onError(const BoostError& error) {
		assert(error);

		std::cerr << m_tag
			<< " Error code = " << error.value()
			<< ". Message: " << error.message();

		auto exceptionPtr = std::make_exception_ptr(std::runtime_error(error.message()));

		m_promise.set_exception(std::move(exceptionPtr));
	}

}
