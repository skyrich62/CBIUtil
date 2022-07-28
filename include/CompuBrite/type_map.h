/**
 * The MIT License (MIT)
 *
 * @copyright
 * Copyright (c) 2020 Rich Newman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file
 * @brief Interface for checkpoints.
*/

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
struct element {
    static auto value(type_tag<typename Pair::first_type>) -> type_tag<typename Pair::second_type>;
};

template<typename... elems>
struct type_map : element<elems>... {
    using element<elems>::value...;

    template<typename K>
    using find = typename decltype(type_map::value(type_tag<K>{}))::type;

    template <typename K >
    static constexpr bool has()
    {
        return std::bool_constant< (std::is_same_v<K, typename elems::first_type> || ...) > ::value;
    }
};

} // namespace CompuBrite

#endif // COMPUBRITE_TYPE_MAP_H_INCLUDED

