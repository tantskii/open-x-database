#include "Session.h"

#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/log/trivial.hpp>

using namespace boost::asio::ip;

namespace omx {

	Session::Session(DatabasePtr database, Socket socket)
		: m_database(std::move(database))
		, m_socket(std::move(socket))
	{}

	void Session::startHandling() {
		constexpr uint32_t kContentLengthSize = sizeof(Request::contentLength);

		auto callback = [this](const BoostError& errorCode, std::size_t bytesTransferred) {
			onContentLengthReceived(errorCode, bytesTransferred);
		};

		boost::asio::async_read(
			*m_socket, m_requestBuffer, boost::asio::transfer_exactly(kContentLengthSize), callback);
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
			<< " Request bytes transferred: " << bytesTransferred;

		m_response = processRequest(m_requestBuffer);

		auto responseSerialized = m_response.serialize();

		auto callback = [this](const BoostError& errorCode, std::size_t bytesTransferred) {
			onResponseSent(errorCode, bytesTransferred);
		};

		boost::asio::async_write(
			*m_socket, boost::asio::buffer(responseSerialized), callback);
	}

	void Session::onResponseSent(const BoostError& errorCode, std::size_t bytesTransferred) {

		if (errorCode) {
			BOOST_LOG_TRIVIAL(error) << __PRETTY_FUNCTION__
				<< " Error code = " << errorCode.value()
				<< ". Message: " << errorCode.message();
		}

		BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__
			<< " Response bytes transferred: " << bytesTransferred;

		onFinish();
	}

	void Session::onFinish() {
		delete this;
	}

	omx::Response Session::processRequest(boost::asio::streambuf& requestBuffer) const {
		auto serializedRequest = std::string(
			std::istreambuf_iterator<char>(&requestBuffer),
			std::istreambuf_iterator<char>());

		auto request = omx::Request();
		request.deserialize(serializedRequest);

		BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__
			<< " Received:"
			<< " Request type = " << static_cast<int>(request.requestType)
			<< " Key = " << request.key.id;

		return m_database->handle(request);
	}

	void Session::onContentLengthReceived(const BoostError& errorCode, size_t numBytes) {
		if (errorCode) {
			BOOST_LOG_TRIVIAL(error) << __PRETTY_FUNCTION__
				<< " Error code = " << errorCode.value()
				<< ". Message: " << errorCode.message();

			onFinish();

			return;
		}

		BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__
			<< " Content length bytes transferred: " << numBytes;

		assert(numBytes == sizeof(Request::contentLength));

		using TSize = decltype(Request::contentLength);
		m_contentLength = *reinterpret_cast<const TSize*>(m_requestBuffer.data().data());

		auto callback = [this](const BoostError& errorCode, std::size_t bytesTransferred) {
			onRequestReceived(errorCode, bytesTransferred);
		};

		boost::asio::async_read(
			*m_socket, m_requestBuffer, boost::asio::transfer_exactly(m_contentLength), callback);
	}

}
