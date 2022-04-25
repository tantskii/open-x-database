#include "Session.h"

#include <boost/asio/read_until.hpp>
#include <boost/asio/write.hpp>
#include <boost/log/trivial.hpp>

using namespace boost::asio::ip;

namespace omx {

	Session::Session(DatabasePtr database, Socket socket)
		: m_database(std::move(database))
		, m_socket(std::move(socket))
	{}

	void Session::startHandling() {
		auto requestCallback = [this](const BoostError& errorCode, std::size_t bytesTransferred) {
			onRequestReceived(errorCode, bytesTransferred);
		};

		// TODO read without until
		boost::asio::async_read_until(*m_socket, m_requestBuffer, '\n', requestCallback);
	}

	void Session::onRequestReceived(const BoostError& errorCode, std::size_t bytesTransferred) {

		if (errorCode) {
			BOOST_LOG_TRIVIAL(error) << __PRETTY_FUNCTION__
				<< " Error code = " << errorCode.value()
				<< ". Message: " << errorCode.message();

			onFinish();

			return;
		}

		BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__
			<< "Request bytes transferred: " << bytesTransferred;

		auto responseCallback = [this](const BoostError& errorCode, std::size_t bytesTransferred) {
			onResponseSent(errorCode, bytesTransferred);
		};

		m_response = processRequest(m_requestBuffer);

		auto responseSerialized = m_response.serialize();

		boost::asio::async_write(
			*m_socket, boost::asio::buffer(responseSerialized), responseCallback);
	}

	void Session::onResponseSent(const BoostError& errorCode, std::size_t bytesTransferred) {

		if (errorCode) {
			BOOST_LOG_TRIVIAL(error) << __PRETTY_FUNCTION__
				<< " Error code = " << errorCode.value()
				<< ". Message: " << errorCode.message();
		}

		BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__
			<< "Response bytes transferred: " << bytesTransferred;

		onFinish();
	}

	void Session::onFinish() {
		delete this;
	}

	omx::Response Session::processRequest(boost::asio::streambuf& requestBuffer) const {
		std::string serializedRequest;

		auto stream = std::istream(&requestBuffer);

		std::getline(stream, serializedRequest);

		auto request = omx::Request();

		request.deserialize(serializedRequest);

		BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__
			<< "Received:"
			<< " Request type = " << static_cast<int>(request.requestType)
			<< " Key = " << request.key.id;

		return m_database->handle(request);
	}

}
