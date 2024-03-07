#ifndef COMPUBRITE_TYPE_MAP_H_INCLUDED
#define COMPUBRITE_TYPE_MAP_H_INCLUDED

#include <type_traits>

namespace CompuBrite {

template<typename T>
struct type_tag {
    using type = T;
};


template<typename K, typename V>
struct pair {
    using first_type = K;
    using second_type = V;
};

template<typename Pair>
struct type_map_element
{
    static auto value(type_tag<typename Pair::first_type>) -> type_tag<typename Pair::second_type>;
};

template<typename... elems>
struct type_map : type_map_element<elems>...
{
    using type_map_element<elems>::value...;

    template<typename K>
    using find = typename decltype(type_map::value(type_tag<K>{}))::type;

    template <typename K >
    static constexpr bool has()
    {
        return std::bool_constant< (std::is_same_v<K, typename elems::first_type> || ...) > ::value;
    }
};

template< typename T >
struct type_set_element
{
    static auto value(type_tag< T >) -> type_tag< T >;
};


template<typename... elems>
struct type_set : type_set_element< elems >...
{
    using type_set_element< elems >::value...;

    template< typename K >
    static constexpr bool has()
    {
        return std::bool_constant< (std::is_same_v<K, elems > || ...) > ::value;
    }
};

} // namespace CompuBrite

#endif // COMPUBRITE_TYPE_MAP_H_INCLUDED

