#pragma once
#define ENABLE_PROFILER //TODO: remove this

#ifdef ENABLE_PROFILER 

	#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
		#define FUNC_SIG __PRETTY_FUNCTION__
	#elif defined(__DMC__) && (__DMC__ >= 0x810)
		#define FUNC_SIG __PRETTY_FUNCTION__
	#elif (defined(__FUNCSIG__) || (_MSC_VER))
		#define FUNC_SIG __FUNCSIG__
	#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
		#define FUNC_SIG __FUNCTION__
	#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
		#define FUNC_SIG __FUNC__
	#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
		#define FUNC_SIG __func__
	#elif defined(__cplusplus) && (__cplusplus >= 201103)
		#define FUNC_SIG __func__
	#else
		#define FUNC_SIG "FUNC_SIG unknown!"
	#endif

    #include <RoxEngine/profiler/cpu_profiler.hpp>
	#include <RoxEngine/profiler/gpu_profiler.hpp>

    #define PROFILER_BEGIN_SESSION(name) ::RoxEngine::Profiler::Instrumentor::Get().BeginSession(name)
	#define PROFILER_END_SESSION() ::RoxEngine::Profiler::Instrumentor::Get().EndSession()
	#define PROFILER_SCOPE_LINE2(name, line) constexpr auto fixedName##line = ::RoxEngine::Profiler::Utils::CleanupOutputString(name, "__cdecl ");\
											   ::RoxEngine::Profiler::InstrumentationTimer timer##line(fixedName##line.Data)
	#define PROFILER_SCOPE_LINE(name, line) PROFILER_SCOPE_LINE2(name, line)
	#define PROFILER_SCOPE(name) PROFILER_SCOPE_LINE(name, __LINE__)
	#define PROFILER_FUNCTION() PROFILER_SCOPE(FUNC_SIG)
#else
	#define PROFILER_BEGIN_SESSION(name)
	#define PROFILER_END_SESSION()
	#define PROFILER_SCOPE(name)
	#define PROFILER_FUNCTION()
#endif