#pragma once

#include <boost/format.hpp>
#include <boost/log/trivial.hpp>

#include <string>

namespace omx::log {

	template <typename... Args>
	void info(const char* str, Args... args) {
		boost::format f(str);
		BOOST_LOG_TRIVIAL(info) << (f % ... % std::forward<Args>(args));
	}

	template <typename... Args>
	void debug(const char* str, Args... args) {
		boost::format f(str);
		BOOST_LOG_TRIVIAL(debug) << (f % ... % std::forward<Args>(args));
	}

	template <typename... Args>
	void warning(const char* str, Args... args) {
		boost::format f(str);
		BOOST_LOG_TRIVIAL(warning) << (f % ... % std::forward<Args>(args));
	}

	template <typename... Args>
	void error(const char* str, Args... args) {
		boost::format f(str);
		BOOST_LOG_TRIVIAL(error) << (f % ... % std::forward<Args>(args));
	}

}
