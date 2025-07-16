#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/async_logger.h>
#include <RoxEngine/core/Logger.hpp>

namespace RoxEngine::log
{
	static Logger sLogger("ROXENGINE-APP");

	Logger::Logger(const std::string& name)
	{
		spdlog::sink_ptr sinks[] = {
			std::make_shared<spdlog::sinks::stdout_color_sink_mt>(),
			std::make_shared<spdlog::sinks::basic_file_sink_mt>(std::format("logs/{}.txt", name), true)
		};
		sinks[0]->set_level(spdlog::level::trace);
		sinks[1]->set_level(spdlog::level::trace);
		sinks[0]->set_pattern("%^[%T] %n: %v%$");
		sinks[1]->set_pattern("[%l] [%T] %n: %v");

		mLogger = CreateRef<spdlog::logger>(name, std::begin(sinks), std::end(sinks));
		mLogger->set_level(spdlog::level::trace);
		spdlog::register_logger(mLogger);
	}

	void Logger::trace(const std::string& msg) {mLogger->trace(msg); }
	void Logger::info(const std::string& msg)  {mLogger->info(msg); }
	void Logger::warn(const std::string& msg)  {mLogger->warn(msg); }
	void Logger::error(const std::string& msg) {mLogger->error(msg); }
	void Logger::fatal(const std::string& msg) {mLogger->critical(msg); }

	void Logger::flush()
	{
		mLogger->flush();
	}

	void trace(const std::string& msg) {
		sLogger.trace(msg);
	}
	void info(const std::string& msg) {
		sLogger.info(msg);
	}
	void warn(const std::string& msg) {
		sLogger.warn(msg);
	}
	void error(const std::string& msg) {
		sLogger.error(msg);
	}
	void fatal(const std::string& msg) {
		sLogger.fatal(msg);
	}

	void flush_all() 
	{
		spdlog::details::registry::instance().apply_all([](auto x) {
			x->flush();
		});
	}
}
