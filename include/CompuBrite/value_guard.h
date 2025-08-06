/**
 * @copyright
 * Copyright (c) 2025 Rich Newman
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

#ifndef COMPUBRITE_VALUE_GUARD_H_INCLUDED
#define COMPUBRITE_VALUE_GUARD_H_INCLUDED

namespace CompuBrite {

/// RAII value guard that temporarily sets a variable and restores it on destruction
/// @tparam T Type of the variable to guard
template<typename T>
class value_guard
{
  private:
    T& target_;
    T original_value_;

  public:
    /// Constructor: save original value and set new value
    /// @param target Reference to the variable to modify
    /// @param new_value Value to set during the object's lifetime
    value_guard(T& target, const T& new_value) 
        : target_(target), original_value_(target)
    {
        target_ = new_value;
    }

    /// Destructor: always restore original value
    ~value_guard() noexcept
    {
        target_ = original_value_;
    }

    /// Reset to a new value during the object's lifetime
    /// @param new_value New value to set (original will still be restored on destruction)
    void reset(const T& new_value)
    {
        target_ = new_value;
    }

    /// Get current value of the target
    /// @return Current value of the target variable
    const T& current() const noexcept
    {
        return target_;
    }

    /// Get original value that will be restored
    /// @return Original value saved at construction time
    const T& original() const noexcept
    {
        return original_value_;
    }

    // Non-copyable and non-movable for safety
    value_guard(const value_guard&) = delete;
    value_guard& operator=(const value_guard&) = delete;
    value_guard(value_guard&&) = delete;
    value_guard& operator=(value_guard&&) = delete;
};

/// Helper function for easier usage with template argument deduction
/// @param target Reference to the variable to modify
/// @param new_value Value to set during the returned object's lifetime
/// @return value_guard object that will restore the original value when destroyed
template<typename T>
auto make_value_guard(T& target, const T& new_value) noexcept
{
    return value_guard<T>(target, new_value);
}

} // namespace CompuBrite

#endif // COMPUBRITE_VALUE_GUARD_H_INCLUDED
