/**
 * @copyright
 * Copyright (c) 2021 Rich Newman
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * @author
 * Rich Newman
 */

#ifndef COMPUBRITE_STRING_RECORD_H_INCLUDED
#define COMPUBRITE_STRING_RECORD_H_INCLUDED

#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>

namespace CompuBrite {

/// A string_record keeps track of unique strings and provides a simple means
/// of referring to them.
class string_record
{
public:
    ~string_record() = default;
    string_record(const string_record &) = default;
    string_record& operator=(const string_record &) = default;

    string_record(string_record &&) = default;
    string_record& operator=(string_record &&) = default;

    /// @return a string_record from the given string.  If there is no record
    /// of that string, create a new string_record and return that.
    static string_record from_string(const std::string &str);

    /// @return the index for this string_record.  This is it's index into
    /// the string repository.
    size_t index() const                         { return _index; }

    /// @return a string_view associated with this string_record
    std::string_view string_view() const         { return _strings[_index]; }

    /// @return a const reference to the string associated with this
    /// string_record
    const std::string& string() const            { return _strings[_index]; }

private:
    /// string_record objects should only be created from the static
    /// factory function: from_string()
    string_record();


private:
    size_t _index;

    using Strings = std::vector<std::string>;
    using Map = std::unordered_map<std::string, string_record>;

    static Strings _strings;
    static Map _map;
};

bool
inline operator==(string_record lhs, string_record rhs)
{
    return lhs.index() == rhs.index();
}

} // namespace CompuBrite

namespace std
{
    template<>
    struct hash<CompuBrite::string_record>
    {
        std::size_t operator()(const CompuBrite::string_record &s) const noexcept
        {
            return std::hash<size_t>{}(s.index());
        }
    };
}

#endif // COMPUBRITE_STRING_RECORD_H_INCLUDED
