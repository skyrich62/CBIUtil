#ifndef COMPUBRITE_INTERNAL_STACK_H_INCLUDED
#define COMPUBRITE_INTERNAL_STACK_H_INCLUDED

#include <memory>
#include "CompuBrite/CheckPoint.h"

/// Manage a stack of elements which is actually kept on the program stack.
///
/// This is a template utility type intended for single-threaded, strictly
/// nested (LIFO) usage. Each instance represents a stack "frame" which:
///  - constructs a T instance via T::create()
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
  /// Requirements:
  ///  - T provides a static std::unique_ptr<T> create() factory.
  ///
  /// Effects:
  ///  - calls T::create() and stores the result in _item
  ///  - caches the raw pointer in _raw
  ///  - links this frame above the previous top() via _prev and _top.
  internal_stack()
  {
    _item = T::create();
    _raw  = _item.get();
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

  /// Return the raw pointer to the managed T instance in this frame.
  ///
  /// The returned pointer is non-owning. It becomes invalid if move()
  /// is called and the moved-to owner destroys the object.
  pointer get() const { return _raw; }

  /// Transfer ownership of the managed T instance out of this frame.
  ///
  /// After this call, _item is empty. The raw pointer returned by get()
  /// may become dangling if the new owner destroys the object before
  /// this frame is destroyed.
  std::unique_ptr<T> move() { return std::move(_item); }

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

  /// Owning storage for the T instance created in the constructor.
  std::unique_ptr<T> _item;

  /// Cached raw pointer to the managed T instance for fast access.
  pointer _raw{};

  /// Per-T top-of-stack pointer for the chain of active frames.
  static Self* _top;
};

template<typename T>
internal_stack<T>* internal_stack<T>::_top = nullptr;

#endif // COMPUBRITE_INTERNAL_STACK_H_INCLUDED
