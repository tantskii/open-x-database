#include <omx/DatabaseConnector.h>

#include "DatabaseConnectorImpl.h"

namespace omx {

	DatabaseConnector::DatabaseConnector(std::string address, uint16_t port)
		: m_impl(std::move(address), port)
	{}

	std::future<omx::Response> DatabaseConnector::execute(const Request& request) {
		return m_impl->execute(request);
	}

	DatabaseConnector::~DatabaseConnector() = default;

}
