#pragma once

#include "Aliases.h"
#include "UUID.h"

#include <omx/Database.h>

#include <boost/asio.hpp>

#include <future>
#include <optional>

namespace omx {

class ClientSession : public std::enable_shared_from_this<ClientSession> {
	public:
		ClientSession(SocketPtr socket, ResolverPtr resolver, QueryPtr query, Request request);

		~ClientSession();

		std::future<omx::Response> run();

	private:
		using Ptr = std::shared_ptr<ClientSession>;

		void onResolve(Ptr self, const BoostError& error, const Resolver::iterator& it);

		void onConnect(Ptr self, const BoostError& error);

		void onSend(Ptr self, const BoostError& error, size_t numBytes);

		void onContentLengthReceive(Ptr self, const BoostError& errorCode, std::size_t numBytes);

		void onReceive(Ptr self, const BoostError& error, size_t numBytes);

		void onError(const BoostError& error);

		using ContentLength = decltype(Response::contentLength);

	private:
		SocketPtr m_socket;
		QueryPtr m_query;
		ResolverPtr m_resolver;
		std::promise<omx::Response> m_promise;

		boost::asio::streambuf m_buffer;
		ContentLength m_contentLength = 0;

		const omx::Request m_request;

		UUID m_tag;
	};

}
