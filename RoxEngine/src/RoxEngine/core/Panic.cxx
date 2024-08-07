module;
#include <functional>
export module roxengine:panic;

export namespace RoxEngine {
	struct IError;

	using PanicFunc = std::function<int(IError&)>;

	// Sets the thread local panic handler
	void SetPanicHandler(PanicFunc func);
	// Sets the global panic handler, different threads will use this if they didnt define their own panic handlers
	void SetGlobalPanicHandler(PanicFunc func);
	
	//TODO: c++20 std::source_location
	void Panic(IError&);
}