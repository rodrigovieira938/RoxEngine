module;
#define SPDLOG_HEADER_ONLY
#define SPDLOG_USE_STD_FORMAT
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/async_logger.h>
module roxengine;

namespace RoxEngine::log
{
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

	void Logger::flush()
	{
		mLogger->flush();
	}
}
