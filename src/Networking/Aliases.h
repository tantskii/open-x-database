#pragma once

#include <omx/Database.h>

#include <boost/asio/ip/tcp.hpp>

#include <memory>

namespace omx {

	using DatabasePtr = std::shared_ptr<Database>;

	using Socket = boost::asio::ip::tcp::socket;
	using SocketPtr = std::shared_ptr<Socket>;

	using Resolver = boost::asio::ip::tcp::resolver;
	using ResolverPtr = std::shared_ptr<Resolver>;

	using Query = boost::asio::ip::tcp::resolver::query;
	using QueryPtr = std::shared_ptr<Query>;

	using BoostError = boost::system::error_code;

}