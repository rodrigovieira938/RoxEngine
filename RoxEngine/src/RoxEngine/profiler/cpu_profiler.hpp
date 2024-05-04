#pragma once

#include <algorithm>
#include <chrono>
#include <fstream>
#include <string>

#include <mutex>
#include <thread>
#include <vector>

#include <unordered_map>

namespace RoxEngine::Profiler {
    namespace Utils {
        template <size_t N>
		struct ChangeResult
		{
			char Data[N];
		};
        template <size_t N, size_t K>
		constexpr auto CleanupOutputString(const char(&expr)[N], const char(&remove)[K])
		{
			ChangeResult<N> result = {};

			size_t srcIndex = 0;
			size_t dstIndex = 0;
			while (srcIndex < N)
			{
				size_t matchIndex = 0;
				while (matchIndex < K - 1 && srcIndex + matchIndex < N - 1 && expr[srcIndex + matchIndex] == remove[matchIndex])
					matchIndex++;
				if (matchIndex == K - 1)
					srcIndex += matchIndex;
				result.Data[dstIndex++] = expr[srcIndex] == '"' ? '\'' : expr[srcIndex];
				srcIndex++;
			}
			return result;
		}
    }

    struct ProfileResult {
        std::string Name;
        long long Start, End;
        uint32_t ThreadID;
        int32_t Parent;
    };

    struct InstrumentationSession {
        std::string Name;
    };


    class Instrumentor {
    private:
        friend class InstrumentationTimer;
        InstrumentationSession *mCurrentSession;
        std::unordered_map<std::thread::id, std::vector<ProfileResult>> mData;
        std::mutex mlock;
        int32_t mCurrentParent;

    public:
        Instrumentor() : mCurrentSession(nullptr) {}

        void BeginSession(const std::string &name) {
            mCurrentParent = -1;
            mCurrentSession = new InstrumentationSession{name};
        }

        void EndSession() {
            delete mCurrentSession;
            mCurrentSession = nullptr;
        }

        void WriteProfile(const ProfileResult &result) {
            std::lock_guard<std::mutex> lock(mlock);
            mCurrentParent--;
            mData[std::this_thread::get_id()].push_back(result);
        }

        std::vector<ProfileResult>& GetData() {
            std::lock_guard<std::mutex> lock(mlock);
            return mData[std::this_thread::get_id()];
        }

        static Instrumentor &Get() {
            static Instrumentor instance;
            return instance;
        }
    };

    class InstrumentationTimer {
    public:
        InstrumentationTimer(const char *name) : mName(name), mStopped(false) {
            mStartTimepoint = std::chrono::high_resolution_clock::now();
            auto& instrumentor = Instrumentor::Get();
            mParent = instrumentor.mCurrentParent;
            instrumentor.mCurrentParent++;
        }

        ~InstrumentationTimer() {
            if (!mStopped)
            Stop();
        }

        void Stop() {
            auto endTimepoint = std::chrono::high_resolution_clock::now();

            long long start = std::chrono::time_point_cast<std::chrono::microseconds>(
                                mStartTimepoint)
                                .time_since_epoch()
                                .count();
            long long end =
                std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint)
                    .time_since_epoch()
                    .count();

            uint32_t threadID =
                std::hash<std::thread::id>{}(std::this_thread::get_id());
            Instrumentor::Get().WriteProfile({mName, start, end, threadID, mParent});

            mStopped = true;
        }
    private:
        const char *mName;
        int32_t mParent;
        std::chrono::time_point<std::chrono::high_resolution_clock> mStartTimepoint;
        bool mStopped;
    };
} // namespace RoxEngine::Profiller