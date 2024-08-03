#pragma once
#include <type_traits>
#include <memory>

// return underlying type or T
template<typename T>
struct get_smart_ptr_underlying_type
{
    template<typename U>
    static U get_type(const std::shared_ptr<U>&);
    template<typename U>
    static U get_type(const std::unique_ptr<U>&);
    template<typename U>
    static U get_type(const std::weak_ptr<U>&);
    static T get_type(...);
	using type = decltype(get_type(std::declval<T>()));
};
template<typename T>
struct is_smart_pointer_impl
{
    template <typename U>
    static std::true_type test(const std::shared_ptr<U>&);
    template <typename U>
    static std::true_type test(const std::unique_ptr<U>&);
    template <typename U>
    static std::true_type test(const std::weak_ptr<U>&);
    static std::false_type test(...);

    using value = decltype(test(std::declval<T>()));
};

template <typename T>
struct is_smart_pointer : is_smart_pointer_impl<T>::value {};