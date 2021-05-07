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
 * @brief Specialization of std::hash over std::pair, and hash related
 * utility functions.
*/

#ifndef COMPUBRITE_HASH_UTIL_H_INCLUDED
#define COMPUBRITE_HASH_UTIL_H_INCLUDED

#include <functional>
#include <utility>

namespace std {

    /// Create a hash for a given type.
    /// @tparam T The type to hash
    /// @param t The object to hash
    /// @return The hash value.
    template <typename T>
    size_t make_hash(const T &t)
    {
        return hash<T>()(t);
    }

    /// Compute a hash value for the a std::pair of a given type.
    /// @tparam T The type for the first member of the pair.
    /// @tparam U the type for the second member of the pair.
    /// @param p The pair to be hashed.
    /// @return The hash value over the pair.
    template <typename T, typename U>
    struct hash< pair<T, U> > {
        size_t operator()(const pair<T, U> &p) const
        {
            return make_hash(p.first) ^ (make_hash(p.second) << 1);
        }
    };
}

#endif // COMPUBRITE_HASH_UTIL_H_INCLUDED
