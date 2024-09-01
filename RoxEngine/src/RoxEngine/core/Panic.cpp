module;
#include <iostream>
#include <functional>
module roxengine;
import :errors;
import :logger;
namespace RoxEngine {

	int DefaultPanicHandler(IError& error) {
		log::fatal("Application panicked: {}", error.GetError());
		return -1;
	}

	static PanicFunc sPanicHandler = DefaultPanicHandler;
	thread_local static PanicFunc sLocalPanicHandler = nullptr;


	void SetPanicHandler(PanicFunc func)
	{
		sLocalPanicHandler = func;
	}

	void SetGlobalPanicHandler(PanicFunc func)
	{
		if (func == nullptr) {
			
		}
	}

	void Panic(IError& error) {
		if (sLocalPanicHandler == nullptr)
			exit(sPanicHandler(error));
		exit(sLocalPanicHandler(error));
	}
	inline void Panic(const IError& error)
	{
		Panic(error);
	}
}