#pragma once

#include <omx/Database.h>

#include <boost/asio/ip/tcp.hpp>

#include <memory>

namespace omx {

	using DatabasePtr = std::shared_ptr<Database>;

	using Socket = std::shared_ptr<boost::asio::ip::tcp::socket>;

	using BoostError = boost::system::error_code;

}