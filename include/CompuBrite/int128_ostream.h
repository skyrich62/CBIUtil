#ifndef int128_ostream_h_included
#define int128_ostream_h_included

#include <ostream>
#include <ios>

// Stream operators for __int128 types should be in global namespace
// for proper ADL (Argument-Dependent Lookup) and compatibility with
// generic code, testing frameworks, and standard library components
std::ostream& operator<<(std::ostream& os, __int128 value);
std::ostream& operator<<(std::ostream& os, unsigned __int128 value);

#endif // int128_ostream_h_included
