#pragma once

#include "Aliases.h"

#include <omx/Database.h>

#include <boost/asio.hpp>

#include <future>
#include <optional>

namespace omx {

	class ClientSession {
	public:
		ClientSession(SocketPtr socket, ResolverPtr resolver, QueryPtr query);

		std::future<omx::Response> run(const omx::Request& request);

		void onResolve(const BoostError& error, const Resolver::iterator& it);

		void onConnect(const BoostError& error);

		void onSend(const BoostError& error, size_t numBytes);

		void onContentLengthReceive(const BoostError& errorCode, std::size_t numBytes);

		void onReceive(const BoostError& error, size_t numBytes);

		void onFinish(std::optional<BoostError> error);

	private:

		using ContentLength = decltype(Response::contentLength);

		SocketPtr m_socket;
		QueryPtr m_query;
		ResolverPtr m_resolver;
		std::promise<omx::Response> m_promise;

		boost::asio::streambuf m_buffer;
		ContentLength m_contentLength = 0;

		omx::Request m_request;
	};

}
