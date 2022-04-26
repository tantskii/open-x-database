#include "DatabaseConnection.h"

#include <iostream>
#include <array>

using boost::asio::ip::tcp;

constexpr auto kUseFuture = boost::asio::use_future;
constexpr uint16_t kBufferSize = 1024;

namespace omx {

	DatabaseConnection::DatabaseConnection(std::string address, uint16_t port)
		: m_address(std::move(address)), m_port(port)
	{
		m_socket = std::make_shared<tcp::socket>(m_service);
		m_thread = std::thread([this] { m_service.run(); });
	}

	DatabaseConnection::~DatabaseConnection() {
		m_service.stop();
		m_thread.join();
	}

	omx::Response DatabaseConnection::execute(const Request& request) {
		try {
			auto resolver = tcp::resolver(m_service);
			auto query = tcp::resolver::query(tcp::v4(), m_address, std::to_string(m_port));

			auto endpoint = resolver.resolve(query);

			boost::asio::connect(*m_socket, endpoint);

			m_socket->send(boost::asio::buffer(request.serialize()));

			std::array<char, kBufferSize> receiveBuffer = {};
			uint32_t numBytes = m_socket->receive(boost::asio::buffer(receiveBuffer));

			auto stream = std::ostringstream();
			stream.write(receiveBuffer.data(), numBytes);

			omx::Response response;
			response.deserialize(stream.str());

			return response;

		} catch (std::system_error& systemError) {
			std::cerr << systemError.what() << std::endl;
			return omx::Response{};
		}
	}
}
