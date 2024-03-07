#ifndef IS_MEMBER_OF_H_INCLUDED
#define IS_MEMBER_OF_H_INCLUDED

#include <variant>
#include <type_traits>

namespace CompuBrite {
template< typename T, typename U> struct is_member_of;

template< typename T, typename... Ts>
struct is_member_of<T, std::variant< Ts... > > :
std::bool_constant< (std::is_same_v< T, Ts > || ...) >
{ };

template< typename T, typename U>
inline constexpr bool is_member_of_v = is_member_of<T, U>::value;

} // namespace CompuBrite

#endif // IS_MEMBER_OF_H_INCLUDED
