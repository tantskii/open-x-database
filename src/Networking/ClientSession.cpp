#include "ClientSession.h"

namespace omx {

	ClientSession::ClientSession(
		SocketPtr socket,
		ResolverPtr resolver,
		QueryPtr query)
		: m_socket(std::move(socket))
		, m_resolver(std::move(resolver))
		, m_query(std::move(query))
	{}

	std::future<omx::Response> ClientSession::run(const Request& request) {
		auto future = m_promise.get_future();

		m_request = request;

		try {
			auto resolveHandler = [this](const BoostError& error, const Resolver::iterator& it) {
				onResolve(error, it);
			};

			m_resolver->async_resolve(*m_query, resolveHandler);
		} catch (const std::exception& e) {
			m_promise.set_exception(std::current_exception());
		}

		return future;
	}

	void ClientSession::onResolve(const BoostError& error, const Resolver::iterator& it) {
		if (error) {
			onFinish(error);
			return;
		}

		auto connectHandler = [this](const BoostError& error) {
			onConnect(error);
		};

		m_socket->async_connect(it->endpoint(), connectHandler);
	}

	void ClientSession::onConnect(const BoostError& error) {
		if (error) {
			onFinish(error);
			return;
		}

		auto sendHandler = [this](const BoostError& error, size_t numBytes) {
			onSend(error, numBytes);
		};

		m_socket->async_send(boost::asio::buffer( m_request.serialize()), sendHandler);
	}

	void ClientSession::onSend(const BoostError& error, size_t numBytes) {
		if (error) {
			onFinish(error);
			return;
		}

		auto readUntilHandler = [this](const BoostError& error, size_t numBytes) {
			onContentLengthReceive(error, numBytes);
		};

		constexpr uint32_t kContentLengthSize = sizeof(Response::contentLength);

		auto matchCondition = boost::asio::transfer_exactly(kContentLengthSize);

		boost::asio::async_read(*m_socket, m_buffer, matchCondition, readUntilHandler);
	}

	void ClientSession::onContentLengthReceive(const BoostError& error, size_t numBytes) {
		if (error) {
			onFinish(error);
			return;
		}

		assert(numBytes == sizeof(ContentLength));

		m_contentLength = *reinterpret_cast<const ContentLength*>(m_buffer.data().data());

		auto matchCondition = boost::asio::transfer_exactly(m_contentLength);

		auto receiveHandler = [this](const BoostError& error, size_t numBytes) {
			onReceive(error, numBytes);
		};

		boost::asio::async_read(*m_socket, m_buffer, matchCondition, receiveHandler);
	}

	void ClientSession::onReceive(const BoostError& error, size_t numBytes) {
		if (error) {
			onFinish(error);
			return;
		}

		assert(numBytes == m_contentLength);

		const auto responseSerialized = std::string(
			std::istreambuf_iterator<char>(&m_buffer),
			std::istreambuf_iterator<char>());

		auto response = omx::Response();
		response.deserialize(responseSerialized);

		m_promise.set_value(std::move(response));

		onFinish(std::nullopt);
	}

	void ClientSession::onFinish(std::optional<BoostError> errorOpt) {
		if (errorOpt.has_value()) {
			auto exceptionPtr = std::make_exception_ptr(std::runtime_error(errorOpt->message()));

			m_promise.set_exception(std::move(exceptionPtr));
		}

		delete this;
	}

}
