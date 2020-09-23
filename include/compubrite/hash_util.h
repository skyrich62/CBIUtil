#ifndef COMPUBRITE_HASH_UTIL_H_INCLUDED
#define COMPUBRITE_HASH_UTIL_H_INCLUDED

#include <functional>
#include <utility>

namespace std {
    template <typename T>
    size_t make_hash(const T &t)
    {
        return hash<T>()(t);
    }

    template <typename T, typename U>
    struct hash< pair<T, U> > {
        size_t operator()(const pair<T, U> &p) const
        {
            return make_hash(p.first) ^ (make_hash(p.second) << 1);
        }
    };
}

#endif // COMPUBRITE_HASH_UTIL_H_INCLUDED
