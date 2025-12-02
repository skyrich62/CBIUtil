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
 * @brief Interface for internal_stack.
*/

#ifndef COMPUBRITE_INTERNAL_STACK_H_INCLUDED
#define COMPUBRITE_INTERNAL_STACK_H_INCLUDED

#include "CompuBrite/CheckPoint.h"

namespace CompuBrite {

/// Manage a stack of non-owning pointers which is actually kept on the
/// program stack.
///
/// This is a template utility type intended for single-threaded, strictly
/// nested (LIFO) usage. Each instance represents a stack "frame" which:
///  - holds a raw T* that is owned elsewhere
///  - links itself into a per-T static singly-linked list headed by top()
///  - automatically restores the previous top() on destruction.
///
/// A CheckPoint::expect is used in the destructor to verify that frames are
/// destroyed in strict LIFO order; misuse will trigger the active
/// CheckPoint policy when CBI_CHECKPOINTS is enabled.
template<typename T>
class internal_stack
{
public:
  using Self    = internal_stack<T>;
  using pointer = T*;

  /// Construct a new stack frame for T and push it on the per-T stack.
  ///
  /// @param ptr A non-owning pointer to an object whose lifetime must
  ///            exceed that of this frame.
  internal_stack(pointer ptr)
    : _ptr(ptr)
  {
    _prev = _top;
    _top  = this;
  }

  /// Destroy this stack frame and pop it from the per-T stack.
  ///
  /// A CheckPoint expectation verifies LIFO destruction order by ensuring
  /// that the current top-of-stack frame is this object. If that expectation
  /// fails and checkpoints are enabled, the configured trap behavior will
  /// be invoked before the stack link is adjusted.
  ~internal_stack()
  {
    CompuBrite::CheckPoint::expect(CBI_HERE,
                                   _top == this,
                                   "internal_stack destroyed out of LIFO order");
    _top = _prev;
  }

  /// @name Disallow copying, moving and heap allocation.
  ///
  /// Frames are tied to program stack scopes and to a single linked chain,
  /// so copying, moving, and heap allocation are all prohibited.
  /// @{
  internal_stack(const Self&) = delete;
  internal_stack(Self&&)      = delete;

  Self& operator=(const Self&) = delete;
  Self& operator=(Self&&)      = delete;

  static void* operator new(std::size_t)    = delete;
  static void* operator new[](std::size_t)  = delete;

  static void  operator delete(void*, std::size_t)   = delete;
  static void  operator delete[](void*, std::size_t) = delete;
  /// @}

  /// Return the raw pointer stored in this frame.
  ///
  /// The returned pointer is non-owning and must not be deleted via this
  /// class. It becomes invalid if the underlying object is destroyed.
  pointer get() const { return _ptr; }

  /// Return the previous frame in the per-T stack, or nullptr if this
  /// is the bottom-most frame.
  Self* prev() const { return _prev; }

  /// Return the current top-of-stack frame pointer for this T, or
  /// nullptr if no frames are active.
  ///
  /// Note: this is process-global per T and is not thread-local.
  static Self* top() { return _top; }

private:
  /// Link to the previous frame in the per-T stack, or nullptr at bottom.
  Self* _prev{nullptr};

  /// Non-owning pointer to the managed T instance.
  pointer _ptr{nullptr};

  /// Per-T top-of-stack pointer for the chain of active frames.
  static Self* _top;
};

template<typename T>
internal_stack<T>* internal_stack<T>::_top = nullptr;

} // namespace CompuBrite

#endif // COMPUBRITE_INTERNAL_STACK_H_INCLUDED
