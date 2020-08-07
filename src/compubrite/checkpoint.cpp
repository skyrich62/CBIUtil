#include "compubrite/checkpoint.h"

#include <algorithm>

namespace CompuBrite {

#ifdef CBI_CHECKPOINTS
bool CheckPoint::init_{false};
bool CheckPoint::all_{false};
bool CheckPoint::disabled_{false};
std::vector<std::string> CheckPoint::categories_;
std::function<void()> CheckPoint::trap_ = [] {};

CheckPoint::CheckPoint(std::string &&category, std::ostream& out) :
    category_(std::move(category)),
    out_(out)

{
    active_ = active(category_);
}

bool CheckPoint::active(const std::string &category)
{
    if (!init_) {
        init();
    }
    if (all_) {
        return true;
    }
    auto found = std::find(categories_.begin(), categories_.end(), category);
    return found != categories_.end();
}

void
CheckPoint::init()
{
    if (init_) {
        return;
    }
    init_ = true;
    auto c = getenv("CBI_CHECKPOINTS");
    if (!c) {
        return;
    }
    std::string ckpts{c};

    if (ckpts.empty()) {
        return;
    }
    auto begin = ckpts.begin();
    auto end   = begin;
    do {
        ++end;
        if (end == ckpts.end() || *end == ':') {
            std::string cat{begin, end};
            if (cat == "*" || cat == "all") {
                all_ = true;
            } else if (cat == "expect-off") {
                disabled_ = true;
            } else if (cat == "expect-crash") {
                trap_ = [] { char *p = nullptr; auto c = *p; };
            } else if (cat == "expect-fatal") {
                trap_ = []
                    {
                        std::cerr << "Aborting" << std::endl;
                        abort();
                    };
            } else {
                categories_.emplace_back(std::move(cat));
            }
            if (end != ckpts.end()) {
                begin = ++end;
            }
        }
    } while (end != ckpts.end());
}

void CheckPoint::trap(const Here& here)
{
    trap_();
}

std::ostream& operator<<(std::ostream& os, const Here &here)
{
    return os << here.file_.filename().string().c_str() << ':' << here.line_ << " (" << here.func_ << ')';
}
#endif // CBI_CHECKPOINTS

} // namespace CompuBrite
