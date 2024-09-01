module;
#define SPDLOG_USE_STD_FORMAT
#include <utility>
#include <spdlog/spdlog.h>
export module roxengine:logger;
import :utils;
namespace RoxEngine::log {
	export class Logger {
	public:
		Logger(const std::string& name);
		void flush();

		template<typename T>
		void trace(const T& msg) { mLogger->trace(std::format("{}", msg)); }
		template<typename T>
		void info(const T& msg)  {mLogger->info(std::format("{}", msg)); }
		template<typename T>
		void warn(const T& msg)  {mLogger->warn(std::format("{}", msg)); }
		template<typename T>
		void error(const T& msg) {mLogger->error(std::format("{}", msg)); }
		template<typename T>
		void fatal(const T& msg) {mLogger->critical(std::format("{}", msg)); }

		template <typename... Args>
		void trace(spdlog::format_string_t<Args...> fmt, Args &&...args) {
			mLogger->trace(std::format(fmt, std::forward<Args>(args)...));
		}
		template <typename... Args>
		void info(spdlog::format_string_t<Args...> fmt, Args &&...args) {
			mLogger->info(std::format(fmt, std::forward<Args>(args)...));
		}
		template <typename... Args>
		void warn(spdlog::format_string_t<Args...> fmt, Args &&...args) {
			mLogger->warn(std::format(fmt, std::forward<Args>(args)...));
		}
		template <typename... Args>
		void error(spdlog::format_string_t<Args...> fmt, Args &&...args) {
			mLogger->error(std::format(fmt, std::forward<Args>(args)...));
		}
		template <typename... Args>
		void fatal(spdlog::format_string_t<Args...> fmt, Args &&...args) {
			mLogger->critical(fmt, std::forward<Args>(args)...);
		}

	private:
		Ref<spdlog::logger> mLogger;
	};

	Logger sLogger("ROXENGINE-APP");

	export template<typename T>
	void trace(const T& msg) { sLogger.trace(std::format("{}",msg)); }
	export template<typename T>
	void info(const T& msg)  { sLogger.info(std::format("{}",msg)); }
	export template<typename T>
	void warn(const T& msg)  { sLogger.warn(std::format("{}",msg)); }
	export template<typename T>
	void error(const T& msg) { sLogger.error(std::format("{}", msg)); }
	export template<typename T>
	void fatal(const T& msg) { sLogger.fatal(std::format("{}", msg)); }

	export template <typename... Args>
		void trace(spdlog::format_string_t<Args...> fmt, Args &&...args) {
		sLogger.trace(std::format(fmt, std::forward<Args>(args)...));
	}
	export template <typename... Args>
	void info(spdlog::format_string_t<Args...> fmt, Args &&...args) {
		sLogger.info(std::format(fmt, std::forward<Args>(args)...));
	}
	export template <typename... Args>
	void warn(spdlog::format_string_t<Args...> fmt, Args &&...args) {
		sLogger.warn(std::format(fmt, std::forward<Args>(args)...));
	}
	export template <typename... Args>
	void error(spdlog::format_string_t<Args...> fmt, Args &&...args) {
		sLogger.error(std::format(fmt, std::forward<Args>(args)...));
	}
	export template <typename... Args>
	void fatal(spdlog::format_string_t<Args...> fmt, Args &&...args) {
		sLogger.fatal(std::format(fmt, std::forward<Args>(args)...));
	}

	//flushes the global logger
	export void flush()
	{
		sLogger.flush();
	}

	export void flush_all()
	{
		spdlog::details::registry::instance().apply_all([](auto x) {
			x->flush();
		});
	}
}