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

#ifndef COMPUBRITE_IS_VALID_H_INCLUDED
#define COMPUBRITE_IS_VALID_H_INCLUDED

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

#endif // COMPUBRITE_IS_VALID_H_INCLUDED
