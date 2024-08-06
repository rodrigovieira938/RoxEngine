#include "Panic.hpp"
#include <RoxEngine/core/Result.hpp>
#include <iostream>

namespace RoxEngine {

	int DefaultPanicHandler(IError& error) {
		std::cerr << "Application panicked: " << error.GetError() << "\n";
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
}