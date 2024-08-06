#pragma once
#include <string>

#include "type_traits.hpp"

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


	template<typename T>
	//Default implementation of ResultDeref
	struct ResultDeref
	{
		using type = T;

		static type* get(T* val) { return val; }
	};

	template<typename U>
	struct ResultDeref<std::shared_ptr<U>>
	{
		using type = U;

		static type* get(std::shared_ptr<U>* val) { return val->get(); }
	};
	template<typename U>
	struct ResultDeref<std::unique_ptr<U>>
	{
		using type = U;

		static type* get(std::unique_ptr<U>* val) { return val->get(); }
	};
	template<typename U>
	struct ResultDeref<std::weak_ptr<U>>
	{
		using type = U;

		static type* get(std::weak_ptr<U>* val) { return val->get(); }
	};
	template<typename Error, typename Value>
	class Result {
	public:
		using underlying_type = typename get_smart_ptr_underlying_type<Value>::type;

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

		typename ResultDeref<Value>::type* operator ->() {
			//TODO: panic (unrecoverable) if error
			return ResultDeref<Value>::get();
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
