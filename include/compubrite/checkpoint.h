#ifndef COMPUBRITE_CHECKPOINT_H
#define COMPUBRITE_CHECKPOINT_H

#include <vector>
#include <string>
#include <ostream>
#include <iostream>
#include <functional>
#include <experimental/filesystem>

namespace CompuBrite {

#ifdef CBI_CHECKPOINTS
#define CBI_HERE CompuBrite::Here(__LINE__, __FILE__, __FUNCTION__)

class Here
{
public:
    friend std::ostream& operator<<(std::ostream &, const Here&);
    Here(unsigned int line, const char* file, const char *func):
        line_(line),
        file_(file),
        func_(func)
    { }
    ~Here() = default;
private:
    unsigned int line_;
    std::experimental::filesystem::path file_;
    const char * func_;
};

class CheckPoint
{
public:
    explicit CheckPoint(std::string &&category, std::ostream& out_ = std::cerr);
    ~CheckPoint() = default;
    CheckPoint(const CheckPoint&) = default;
    CheckPoint(CheckPoint&&) = default;

    CheckPoint& operator=(const CheckPoint&) = default;
    CheckPoint& operator=(CheckPoint&&) = default;

    bool active() const                { return active_; }

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

    template <typename ...Args>
    void print(const Here& here, const Args& ...args)
    {
        if (!active_) {
            return;
        }
        out(here, out_, category_, args...);
    }

    template <typename Cond, typename ...Args>
    static Cond expect(const Here& here, Cond &&cond, const Args& ...args)
    {
        if (!init) {
            init();
        }
        if (!disabled_ && !static_cast<bool>(cond)) {
            out(here, std::cerr, "Expectation failed", args...);
            trap(here);
        }
        return std::forward<Cond>(cond);
    }

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
    static std::vector<std::string> categories_;
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
};
#endif // CBI_CHECKPOINTS

} // namespace CompuBrite



#endif // COMPUBRITE_CHECKPOINT_H
