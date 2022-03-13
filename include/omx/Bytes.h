#pragma once

#include <string>

namespace omx {

	class Bytes {
	public:
		template<typename T>
		void from(const T& value);

		template<typename T>
		void from(const T* data, size_t size);

		template<typename T>
		auto as();

		[[nodiscard]] size_t bytesSize() const;

		std::string& toString();

		[[nodiscard]] const std::string& toString() const;

		[[nodiscard]] const char* toCString() const;

		void clear();

		[[nodiscard]] bool empty() const;

	private:
		std::string m_data;
	};

	template<typename T>
	void Bytes::from(const T& value) {
		const size_t bytesSize =
			value.size() * sizeof(std::remove_pointer_t<decltype(value.data())>);

		m_data.resize(bytesSize);

		std::copy(
			reinterpret_cast<const char*>(value.data()),
			reinterpret_cast<const char*>(value.data()) + bytesSize,
			m_data.begin());
	}


	template<typename T>
	void Bytes::from(const T* data, size_t size) {
		const size_t bytesSize = size * sizeof(std::remove_pointer_t<decltype(data)>);

		m_data.resize(bytesSize);

		std::copy(
			reinterpret_cast<const char*>(data),
			reinterpret_cast<const char*>(data) + bytesSize,
			m_data.begin());
	}

	template<typename T>
	auto Bytes::as() {
		return reinterpret_cast<T*>(m_data.data());
	}

} // namespace omx