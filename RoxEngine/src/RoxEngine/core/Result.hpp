#pragma once
#include <string>

namespace RoxEngine {
	struct IError {
		virtual ~IError() = default;
		virtual std::string_view GetError() = 0;
	};
	class RuntimeError : IError{
	public:
		~RuntimeError() override = default;
		RuntimeError(const std::string&) noexcept;
		RuntimeError(const RuntimeError& other) noexcept;
		RuntimeError& operator=(const RuntimeError& other) noexcept;

		std::string_view GetError() override;

	private:
		std::string mMessage;
	};

	template<typename Error, typename Value>
	class Result {
	public:
		static_assert(std::is_base_of<IError, Error>(), "Result<Error,...> must inherit from IError");
		static_assert(!std::is_same<Error, Value>(), "Result<Error, Value> must not be the same");
		Result(const Error& error) : mIsError(true), mError(error) {}
		Result(const Value& value) : mIsError(false), mValue(value) {}
		~Result() { if (mIsError) { mError.~Error(); } else { mValue.~Value(); } }

		Result& operator =(const Value& val) { mValue = Value(val); mIsError = false; return *this; }
		Result& operator =(const Error& val) { mValue = Error(val); mIsError = true; return *this; }

		bool IsOk() const { return !mIsError; }
		bool IsError() const { return mIsError; }

		Error& GetError() const { return mError; }
		Value& GetValue() const { return mValue; }

		Value* operator ->()
		{
			//TODO: panic (unrecoverable)
			return &mValue;
		}
	private:
		bool mIsError;
		union
		{
			Error mError;
			Value mValue;
		};
	};
}