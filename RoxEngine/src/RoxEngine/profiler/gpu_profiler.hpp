#pragma once
#include <RoxEngine/profiler/cpu_profiler.hpp>
#include <glad/gl.h>
namespace RoxEngine::Profiler {
    void glad_prefunc(const char *name, GLADapiproc, int, ...);
    void glad_posfunc(void*,const char *name, GLADapiproc, int, ...);

    #ifdef PROFILER_IMPLEMENTATION
    InstrumentationTimer* currentTimer;
    void glad_prefunc(const char *name, GLADapiproc, int, ...) {
        if(currentTimer)
            glad_posfunc(nullptr, name, nullptr,0);

        currentTimer = new InstrumentationTimer(name);
    }
    void glad_posfunc(void*,const char *name, GLADapiproc, int, ...) {
        delete currentTimer;
        currentTimer = nullptr;
    }
    #endif
}