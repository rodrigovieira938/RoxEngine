#pragma once
#include <format>
#include <RoxEngine/utils/Utils.hpp>
namespace spdlog {
	class logger;
}
namespace RoxEngine::log {
	class Logger {
	public:
		Logger(const std::string& name);
		void flush();

		void trace(const std::string& msg);
		void info(const std::string& msg);
		void warn(const std::string& msg);
		void error(const std::string& msg);
		void fatal(const std::string& msg);

		template<typename T>
		inline void trace(const T& msg) {trace(std::format("{}", msg)); }
		template<typename T>
		inline void info(const T& msg)  {info(std::format("{}", msg)); }
		template<typename T>
		inline void warn(const T& msg)  {warn(std::format("{}", msg)); }
		template<typename T>
		inline void error(const T& msg) {error(std::format("{}", msg)); }
		template<typename T>
		inline void fatal(const T& msg) {critical(std::format("{}", msg)); }

		template <typename... Args>
		inline void trace(std::format_string<Args...> fmt, Args &&...args) {
			trace(std::vformat(fmt.get(), std::make_format_args(args...)));
		}
		template <typename... Args>
		inline void info(std::format_string<Args...> fmt, Args &&...args) {
			info(std::vformat(fmt.get(), std::make_format_args(args...)));
		}
		template <typename... Args>
		inline void warn(std::format_string<Args...> fmt, Args &&...args) {
			warn(std::vformat(fmt.get(), std::make_format_args(args...)));
		}
		template <typename... Args>
		inline void error(std::format_string<Args...> fmt, Args &&...args) {
			error(std::vformat(fmt.get(), std::make_format_args(args...)));
		}
		template <typename... Args>
		inline void fatal(std::format_string<Args...> fmt, Args &&...args) {
			fatal(std::vformat(fmt.get(), std::make_format_args(args...)));
		}

	private:
		Ref<spdlog::logger> mLogger;
	};

	void trace(const std::string& msg);
	void info(const std::string& msg);
	void warn(const std::string& msg);
	void error(const std::string& msg);
	void fatal(const std::string& msg);

	template<typename T>
	void trace(const T& msg) { trace(std::format("{}",msg)); }
	template<typename T>
	void info(const T& msg)  { info(std::format("{}",msg)); }
	template<typename T>
	void warn(const T& msg)  { warn(std::format("{}",msg)); }
	template<typename T>
	void error(const T& msg) { error(std::format("{}", msg)); }
	template<typename T>
	void fatal(const T& msg) { fatal(std::format("{}", msg)); }

	template <typename... Args>
	void trace(std::format_string<Args...> fmt, Args &&...args) {
		trace(std::vformat(fmt.get(), std::make_format_args(args...)));
	}
	template <typename... Args>
	void info(std::format_string<Args...> fmt, Args &&...args) {
		info(std::vformat(fmt.get(), std::make_format_args(args...)));
	}
	template <typename... Args>
	void warn(std::format_string<Args...> fmt, Args &&...args) {
		warn(std::vformat(fmt.get(), std::make_format_args(args...)));
	}
	template <typename... Args>
	void error(std::format_string<Args...> fmt, Args &&...args) {
		error(std::vformat(fmt.get(), std::make_format_args(args...)));
	}
	template <typename... Args>
	void fatal(std::format_string<Args...> fmt, Args &&...args) {
		fatal(std::vformat(fmt.get(), std::make_format_args(args...)));
	}

	//flushes the global logger
	void flush();
	void flush_all();
}