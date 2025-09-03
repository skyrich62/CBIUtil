#ifndef int128_istream_h_included
#define int128_istream_h_included

#include <istream>
#include <ios>

// Stream operators for __int128 types should be in global namespace
// for proper ADL (Argument-Dependent Lookup) and compatibility with
// generic code, testing frameworks, and standard library components
std::istream& operator>>(std::istream& is, __int128& value);
std::istream& operator>>(std::istream& is, unsigned __int128& value);

#endif // int128_istream_h_included
