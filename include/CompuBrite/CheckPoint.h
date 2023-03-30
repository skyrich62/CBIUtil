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

#ifndef COMPUBRITE_CHECKPOINT_H_INCLUDED
#define COMPUBRITE_CHECKPOINT_H_INCLUDED

#include <unordered_set>
#include <string>
#include <ostream>
#include <iostream>
#include <functional>

namespace CompuBrite {

/// A ScopeGuard holds a functor which will be executed when it is destroyed,
/// (goes out of scope).
class ScopeGuard
{
public:
    using FunctionType = std::function<void(void)>;

    /// Construct the ScopeGuard with the functor object.
    ScopeGuard(FunctionType &&fn) : fn_{std::move(fn)} { }

    /// Destroy the ScopeGuard, execute the functor.
    ~ScopeGuard()                                      { fn_(); }
private:
    FunctionType fn_;
};

#ifdef CBI_CHECKPOINTS

/// @brief A handy way of helping to describe the current source location.
///
/// It is used in calls to the various CheckPoint printing functions.
/// This creates a Here object at the current point in the source code. For
/// examples, see CheckPoint::hit(), CheckPoint::expect(), and
/// CheckPoint::print().
#define CBI_HERE CompuBrite::Here(__LINE__, __FILE__, __FUNCTION__)

/// @brief This class encapsulates the current source code location at runtime.
class Here
{
public:
    friend std::ostream& operator<<(std::ostream &, const Here&);

    /// @brief Construct a Here object.
    /// @param line The current source code line number
    /// @param file The current source code filename
    /// @param func The name of the currently running function.
    Here(size_t line, const char* file, const char *func):
        file_(file),
        func_(func),
        line_(line)
    { }
    ~Here() = default;
private:
    const char * const file_;
    const char * const func_;
    const size_t       line_;
};

/// @brief Checkpoints are used to print out debugging or exception
/// information.
///
/// There are currently two kinds of CheckPoints: debugging and exception.
/// The behavior of CheckPoints is controlled at compile time by the
/// use of the CBI_CHECKPOINTS macro.  If it is not defined, all CheckPoint
/// objects are disabled and automatically removed by the compiler.
/// Additionally, CheckPoints are also controlled at run time by the
/// CBI_CHECKPOINTS environment variable.
///
/// The CBI_CHECKPOINTS environment variable is a list of categories, separated
/// by colons, (e.g. CBI_CHECKPOINTS=category1:category2)
/// CheckPoints are only enabled at runtime if their corresponding category
/// is enabled.  The following predefined categories exist: "*" or "all"
/// will enable all categories, "expect-off" will disable all exception
/// CheckPoints.  "expect-fatal", will cause all exception CheckPoints to
/// call abort(), and end the program.  "expect-crash" will cause all exception
/// CheckPoints to produce a crash, (by deliberately dereferencing a null
/// pointer)
///
/// Debugging CheckPoints are non-static members and have categories assigned
/// to them at construction time. They are typically used to output debugging
/// messages.  Exception CheckPoints are static members and are controlled
/// only by "except-off", "except-fatal" and "except-crash".  They are used
/// to signal improper or unexpected program execution.
///
/// All of the above assumes that the compile-time macro CBI_CHECKPOINTS has
/// also been defined, (e.g. g++ -g -DCBI_CHECKPOINTS foo.cc -o foo)
/// @par Example
/// Assuming a program ("app), has been written using CheckPoints with
/// categories "foo", "bar" and "zork", one could start such a program as
/// follows:
/// @code
///    $ CBI_CHECKPOINTS="foo:bar:expect-fatal" app -v
/// @endcode
/// This will enable categories "foo" and "bar", but disable category "zork",
/// and will make all exceptions fatal.
/// @par Example
/// @include test.cpp
class CheckPoint
{
public:
    /// @brief Construct a Checkpoint.
    /// @param category The category name for this checkpoint.  This CheckPoint
    /// will only function if this category has been enabled.
    /// @param out_ The output stream to write CheckPoint messages to.
    explicit CheckPoint(std::string &&category, std::ostream& out_ = std::cerr);
    ~CheckPoint() = default;
    CheckPoint(const CheckPoint&) = default;
    CheckPoint(CheckPoint&&) = default;

    CheckPoint& operator=(const CheckPoint&) = default;
    CheckPoint& operator=(CheckPoint&&) = default;

    /// @brief An exception CheckPoint used to always cause an exception
    /// message to be put out, possibly causing program termination or
    /// deliberate crash.
    ///
    /// @param here A Here object describing the current source location,
    /// (this is most easily provided at call time by use of CBI_HERE macro).
    /// @param args The argument values to be printed.
    /// @par Example
    /// You can use CheckPoint::hit() to output a message when the program
    /// reaches an unexpected location.  This is particularly useful in
    /// the default case of switch statements.
    /// @code
    ///     switch(c) {
    ///     case 1:
    ///         doSomething();
    ///         break;
    ///     case 2:
    ///         doSomethingElse();
    ///         break;
    ///     . . .
    ///     default:
    ///         CheckPoint::hit(CBI_HERE, "Shouldn't have gotten here, c= ", c);
    ///         break;
    //      }
    /// @endcode
    /// @see print()
    /// @note
    /// hit() differs from print(), in that hit() is a static member function
    /// and therefore doesn't require an active CheckPoint object.  It is
    /// typically used for program exceptions, not debugging messages.
    template <typename ...Args>
    static void hit(const Here& here, const Args& ...args)
    {
        if (!init_) {
            init();
        }
        if (disabled_) {
            return;
        }
        out(here, std::cerr, "Exception", args...);
        trap(here);
    }

    /// @brief An exception CheckPoint used for program assertion.
    /// @param here A Here object describing the current source location,
    /// (this is most easily provided at call time by use of CBI_HERE macro).
    /// @param cond A condition to be checked. If the condition is false,
    /// and exception CheckPoints are enabled, then the arguments will
    /// be printed after a brief message explaining the exception and
    /// source location.
    /// @param args Arguments to be printed if "cond" is false.
    /// @par Example
    /// You can check for a null pointer where one is not expected as follows:
    /// @code
    ///      CheckPoint::expect(CBI_HERE, ptr, "ptr should not be null!");
    /// @endcode
    /// At runtime, if "ptr" is null a message will be put out stating the
    /// current filename, line number and function name and "ptr should not be
    /// null!".
    /// @return "cond" will be perfectly forwarded back to the caller such
    /// that expect() can be used as an argument to a function call as follows:
    /// @code
    ///     void foo(Foo *p);
    ///     Foo *getFoo();
    ///      ...
    ///     foo(CheckPoint::expect(CBI_HERE, getFoo(), "getFoo() failed!"));
    /// @endcode
    template <typename Cond, typename ...Args>
    static Cond expect(const Here& here, Cond &&cond, const Args& ...args)
    {
        if (!init_) {
            init();
        }
        if (!disabled_ && !static_cast<bool>(cond)) {
            out(here, std::cerr, "Expectation failed", args...);
            trap(here);
        }
        return std::forward<Cond>(cond);
    }

    /// @brief Used as a post-condition to ensure certain values are
    /// maintained.
    /// @param here A Here object describing the current source location.
    /// (this is easily provided at call time by the use of CBI_HERE macro.)
    /// @param cond a condition to be checked at scope termination.  If
    /// the condition is false, and exception CheckPoints are enabled, then
    /// the arguments will be printed after a brief message explaining the
    /// exception and source location.
    /// @param args Arguments to be printed if "cond" is false
    /// @par Example
    /// You can check if a post condition is false at scope termination
    /// as follows:
    /// @code
    /// {
    ///     int j = 42;
    ///     auto var = CheckPoint::ensure(CBI_HERE, j == 42, "J was changed: ", std::ref(j));
    ///     j = 24;
    /// }
    /// @endcode
    template <typename Cond, typename ...Args>
    static ScopeGuard ensure(const Here& here,
                             const Cond &cond,
                             const Args& ...args)
    {
        if (!init) {
            init();
        }
        auto tup = std::make_tuple(args...);
        auto f = [here, cond, tup]() {
           if (!disabled_ && !cond()) {
               out(here, std::cerr, "Invariant not satisfied", tup);
               trap(here);
           }
        };
        return ScopeGuard(std::move(f));
    }

    /// @brief Used to print a debugging message in an active debug CheckPoint
    /// If this debug CheckPoint is active, then the parameters are all
    /// printed, following a brief message describing the checkpoint
    /// category and the current source code location.
    /// @param here A Here object describing the current source location,
    /// (this is most easily provided at call time by use of CBI_HERE macro).
    /// @param args The argument values to be printed.
    /// @par Example
    /// You can also output debugging messages at will as follows:
    /// @code
    ///      CheckPoint dbg("zork");
    ///      ...
    ///      dbg.print(CBI_HERE, "state = ", state);
    /// @endcode
    /// If category "zork" is enabled, (e.g. CBI_CHECKPOINTS="zork" or
    /// CBI_CHECKPOINTS="all"), then a message will be output.
    /// @see hit()
    /// @note
    /// print() differs from hit(), in that print is a non-static member
    /// function, and thus requires an active CheckPoint object to function.
    /// It is typically used for outputting debugging information, rather than
    /// to signal abnormal or unexpected program execution.
    template <typename ...Args>
    void print(const Here& here, const Args& ...args)
    {
        if (!active_) {
            return;
        }
        out(here, out_, category_, args...);
    }

    /// @brief Is this debugging CheckPoint active?
    /// This is useful for adding some extra debugging code if a particular
    /// CheckPoint is enabled.
    /// @par Example
    /// For example a server program could be made to hang until a debugger
    /// has attached as follows:
    /// @code
    ///      CheckPoint dbg("server-hang");
    ///      if (dbg.active()) {
    ///          static bool hang = true;
    ///          while (hang)
    ///              ;
    ///      }
    /// @endcode
    /// If the "server-hang" CheckPoint is enabled, when execution comes
    /// to this point, the program will enter an infinite loop.  Now, a
    /// to this point, the program will enter an infinite loop.  Now, a
    /// developer can attach to the running program with a debugger, set some
    /// breakpoints, examine memory, then finally set "hang" to false and
    /// resume execution.  In release mode, (where the CBI_CHECKPOINT macro
    /// is not defined), active() return false; the compiler will optimize
    /// the entire if statement away, cleanly.
    bool active() const                { return active_ && !disabled_; }


    /// Programatically enable a specific category.  Usually this would
    /// be called from a debugger.  Does nothing if the category is already
    /// eanbled.
    /// @param category The category to enable.
    static void enable(const char *category);

    /// Programatically disabled a specific category.  Usually this would
    /// be called from a debugger. Does nothing if the category is already
    /// disabled.
    /// @param category The category to disable.
    static void disable(const char *category);
private:
    static void init();
    static bool active(const std::string &category);

    template <typename ...Args>
    static void out(const Here &here, std::ostream& out, const std::string &reason, const Args& ...args)
    {
        out << "@@@ CheckPoint (" << reason << "): " << here << "\n@@@ ";
        (out << ... << args);
        out << std::endl;
    }

    static void trap(const Here& here);

private:
    bool active_{false};
    std::string category_;
    std::ostream &out_;

    static bool init_;
    static bool all_;
    static bool disabled_;
    static std::unordered_set<std::string> categories_;
    static std::function<void()> trap_;
};
#else // !CBI_CHECKPOINTS

#define CBI_HERE CompuBrite::Here()
class Here
{
public:
    Here() { }
};

class CheckPoint
{
public:
    CheckPoint(const std::string&, std::ostream& = std::cerr) { }
    ~CheckPoint() = default;
    CheckPoint(const CheckPoint&) = default;
    CheckPoint(CheckPoint&&) = default;

    CheckPoint& operator=(const CheckPoint&) = default;
    CheckPoint& operator=(CheckPoint&&) = default;

    bool active() const                { return false; }

    template <typename ...Args>
    static void hit(const Here& here, const Args& ...args)
    {
    }

    template <typename ...Args>
    void print(const Here& here, const Args& ...args)
    {
    }

    template <typename Cond, typename ...Args>
    static Cond expect(const Here& here, Cond &&cond, const Args& ...args)
    {
        return std::forward<Cond>(cond);
    }

    static void enable(const char *)
    {
    }

    static void disable(const char *)
    {
    }
};
#endif // CBI_CHECKPOINTS


template<typename ...Args>
std::ostream &operator<<(std::ostream& os, const std::tuple<Args...> &t)
{
    std::apply([&os](auto &&... args) { ((os << args), ...);}, t);
    return os;
}

} // namespace CompuBrite


/// Concatenate two preprocessor symbols.  This must be broken up into two
/// macros because of the way the preprocessor works.
#define CBI_CONCATENATE_IMPL(s1, s2) s1##s2
#define CBI_CONCATENATE(s1, s2) CBI_CONCATENATE_IMPL(s1, s2)

/// Create an anonymous name using the current source line and the given
/// symbol.
#define CBI_ANONYMOUS_VARIABLE(str) CBI_CONCATENATE(str, __LINE__)

/// Create an invariant CheckPoint object by wrapping the condition in
/// a lambda, and calling CompuBrite::Checkpoint::ensure().
#define CBI_INVARIANT(here, cond, Args...) \
    auto CBI_ANONYMOUS_VARIABLE(guard_) = CompuBrite::CheckPoint::ensure(here, \
    [&]() { return cond; }, Args);


#endif // COMPUBRITE_CHECKPOINT_H_INCLUDED
