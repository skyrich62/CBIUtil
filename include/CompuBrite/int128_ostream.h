#ifndef int128_ostream_h_included
#define int128_ostream_h_included

#include <ostream>
#include <ios>

namespace CompuBrite {

// Forward declarations
std::ostream& operator<<(std::ostream& os, __int128 value);
std::ostream& operator<<(std::ostream& os, unsigned __int128 value);

} // namespace CompuBrite

#endif // int128_ostream_h_included
