module;
#include <memory>
export module roxengine:result;
import :errors;
import :panic;
export namespace RoxEngine {
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
		static_assert(std::is_base_of<IError, Error>(), "Result<Error,...> must inherit from IError");
		static_assert(!std::is_same<Error, Value>(), "Result<Error, Value> must not be the same");
		Result(const Error& error) : mIsError(true), mError(error) {}
		Result(const Value& value) : mIsError(false), mValue(value) {}
		~Result() { if (mIsError) { mError.~Error(); } else { mValue.~Value(); } }

		Result& operator =(const Value& val) { mValue = Value(val); mIsError = false; return *this; }
		Result& operator =(const Error& val) { mValue = Error(val); mIsError = true; return *this; }

		bool IsOk() const { return !mIsError; }
		bool IsError() const { return mIsError; }

		Error& GetError() const
		{
			if(!mIsError)
				Panic(sResultErrorAcessedError);
			return mError;
		}
		Value& GetValue() const
		{
			if (mIsError)
				Panic(sResultValueAcessedError);
			return mValue;
		}

		typename ResultDeref<Value>::type* operator ->() {
			if (mIsError)
				Panic(sResultValueAcessedError);
			return ResultDeref<Value>::get(&mValue);
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
