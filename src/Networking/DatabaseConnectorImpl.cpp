#include "DatabaseConnectorImpl.h"

#include <iostream>
#include <array>

using boost::asio::ip::tcp;

namespace omx {

	DatabaseConnectorImpl::DatabaseConnectorImpl(std::string address, uint16_t port)
		: m_address(std::move(address)), m_port(port), m_work(m_service)
	{
		m_thread = std::thread([this] { m_service.run(); });
	}

	DatabaseConnectorImpl::~DatabaseConnectorImpl() {
		m_service.stop();
		m_thread.join();
	}

	std::future<omx::Response> DatabaseConnectorImpl::execute(const Request& request) {
		try {
			auto socket   = std::make_shared<Socket>(m_service);
			auto resolver = std::make_shared<Resolver>(m_service);
			auto query    = std::make_shared<Query>(tcp::v4(), m_address, std::to_string(m_port));
			auto* session = new ClientSession(socket, resolver, query);

			return session->run(request);
		} catch (const std::exception& e) {
			auto promise = std::promise<omx::Response>();

			promise.set_exception(std::current_exception());

			return promise.get_future();
		}
	}

}
