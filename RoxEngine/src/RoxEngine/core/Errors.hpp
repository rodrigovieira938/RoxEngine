#pragma once
#include <string>
#define MESSAGE_ERROR(name,msg) struct name : public IError { ~name() override = default; std::string_view GetError() override {return msg;}}; static name s ## name = {};
namespace RoxEngine {
	struct IError {
		virtual ~IError() = default;
		virtual std::string_view GetError() = 0;
	};
	class RuntimeError : IError {
	public:
		~RuntimeError() override = default;
		RuntimeError(const std::string&) noexcept;
		RuntimeError(const RuntimeError& other) noexcept;
		RuntimeError& operator=(const RuntimeError& other) noexcept;

		std::string_view GetError() override;

	private:
		std::string mMessage;
	};

	MESSAGE_ERROR(ResultValueAcessedError, "Result's value accessed while being null")
	MESSAGE_ERROR(ResultErrorAcessedError, "Result's error accessed while being null")
	MESSAGE_ERROR(EntityNotAlive, "Entity is not alive!")
	MESSAGE_ERROR(GetComponentNo, "Entity doesn't have that component!")
	MESSAGE_ERROR(AddComponentTwice, "Adding a component twice to an entity!")
	MESSAGE_ERROR(RemoveComponentNo, "Removing an unexisting component!")
}
#undef MESSAGE_ERROR