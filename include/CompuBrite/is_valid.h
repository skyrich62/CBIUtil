#ifndef IS_VALID_H_INCLUDED
#define IS_VALID_H_INCLUDED

#include <variant>
#include <type_traits>

namespace CompuBrite {
template< typename T, typename U> struct is_valid;

template< typename T, typename... Ts>
struct is_valid<T, std::variant< Ts... > > :
std::bool_constant< (std::is_same_v< T, Ts > || ...) >
{ };

template< typename T, typename U>
inline constexpr bool is_valid_v = is_valid<T, U>::value;

} // namespace CompuBrite

#endif // IS_VALID_H_INCLUDED
