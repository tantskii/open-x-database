#include <omx/DatabaseConnector.h>

#include "DatabaseConnectorImpl.h"

namespace omx {

	DatabaseConnector::DatabaseConnector(std::string address, uint16_t port)
		: m_impl(std::move(address), port)
	{}

	std::future<omx::Response> DatabaseConnector::execute(Request request) {
		return m_impl->execute(std::move(request));
	}

	DatabaseConnector::~DatabaseConnector() = default;

}
