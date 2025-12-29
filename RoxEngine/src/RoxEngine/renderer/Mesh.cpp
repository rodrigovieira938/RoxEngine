#include <RoxEngine/renderer/Mesh.hpp>
#include <type_traits>

/*

*/
#define _DEFINE_HAS_FIELD_ALIAS(field, max, ...)              \
template<typename, typename = void> \
struct has_ ##field : std::conditional<(max >= (get_or_0<__VA_ARGS__>::value+1)), std::false_type, std::true_type>::type {};   \
template<typename T>                              \
struct has_ ##field<T, std::void_t<decltype(std::declval<T>().field)>> : std::true_type {};

#define DEFINE_HAS_FIELD(field, ...) _DEFINE_HAS_FIELD_ALIAS(field ##__VA_ARGS__, field ## _MAX, __VA_ARGS__)

#define REPEAT_FIELD_VARIANTS(x, field) x(field)     x(field, 1)  x(field, 2)  \
                           x(field, 3)  x(field, 4)  x(field, 5)  \
                           x(field, 6)  x(field, 7)  x(field, 8)  \
                           x(field, 9)  x(field, 10) x(field, 11) \
                           x(field, 12) x(field, 13) x(field, 14) \
                           x(field, 15) x(field, 16) x(field, 17)
#define REPEAT_FIELD_VARIANTS_MAX 17

#define REPEAT_FIELDS(x, ...) x(__VA_ARGS__, uv)
#define REPEAT_FIELDS_VARIANTS(x) REPEAT_FIELDS(REPEAT_FIELD_VARIANTS, x)

#define uv_MAX ::RoxEngine::ShaderReflection::VertexBindingPoint::UV_MAX

#define CHECK_FIELD(field, ...) static_assert(has_ ##field ##__VA_ARGS__<Mesh::Data>::value, "Missing " #field #__VA_ARGS__ " alias in RoxEngine::Mesh::Data");
#define CHECK_REPEAT_FIELD_VARIANTS_MAX_CASES(___, ...) static_assert( \
    __VA_ARGS__ ## _MAX <= (REPEAT_FIELD_VARIANTS_MAX + 1), "Add more to REPEAT_FIELD_VARIANTS in " __FILE__);


namespace RoxEngine {
    namespace _debug {
        template<size_t n = 0>
        struct get_or_0 {
            static constexpr auto value = n; 
        };
        REPEAT_FIELDS(CHECK_REPEAT_FIELD_VARIANTS_MAX_CASES)
        REPEAT_FIELDS_VARIANTS(DEFINE_HAS_FIELD)
        REPEAT_FIELDS_VARIANTS(CHECK_FIELD)
    }

    Mesh::Data::Data() {
        new(&uv) FieldAliasWorkaround<std::vector<glm::vec2>, ShaderReflection::VertexBindingPoint::UV_MAX, 0>();
    }
}