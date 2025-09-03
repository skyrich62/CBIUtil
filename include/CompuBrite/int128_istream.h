#ifndef int128_istream_h_included
#define int128_istream_h_included

#include <istream>
#include <ios>

namespace CompuBrite {

// Forward declarations
std::istream& operator>>(std::istream& is, __int128& value);
std::istream& operator>>(std::istream& is, unsigned __int128& value);

} // namespace CompuBrite

#endif // int128_istream_h_included
