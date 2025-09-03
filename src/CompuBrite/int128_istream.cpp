#include "CompuBrite/int128_istream.h"
#include <cctype>
#include <ios>

namespace CompuBrite {

// Helper function to skip whitespace and get first non-whitespace character
static bool skip_whitespace_and_get(std::istream& is, char& c) {
    while (is.get(c) && std::isspace(c)) {
        // continue
    }
    return static_cast<bool>(is);
}

// Helper function to handle sign parsing
static bool parse_sign(std::istream& is, char& c, bool& negative, bool allow_negative = true) {
    negative = false;

    if (c == '-') {
        if (!allow_negative) {
            is.unget();
            is.setstate(std::ios::failbit);
            return false;
        }
        negative = true;
        if (!is.get(c)) {
            is.setstate(std::ios::failbit);
            return false;
        }
    } else if (c == '+') {
        if (!is.get(c)) {
            is.setstate(std::ios::failbit);
            return false;
        }
    }

    return true;
}

// Helper function to determine base and handle prefixes
static int determine_base_and_advance(std::istream& is, char& c, std::ios::fmtflags flags) {
    // If specific base is set via flags, use it
    if (flags & std::ios::hex) {
        return 16;
    }
    if (flags & std::ios::oct) {
        return 8;
    }

    // Auto-detect base based on prefix
    if (c != '0') {
        return 10;  // Decimal
    }

    // Could be octal or hex, need to look ahead
    char next;
    if (!is.get(next)) {
        // Just a single '0'
        return 10;
    }

    if (next == 'x' || next == 'X') {
        // Hexadecimal
        if (!is.get(c)) {
            is.setstate(std::ios::failbit);
            return -1;
        }
        return 16;
    } else {
        // Put back the character and treat as octal
        is.unget();
        return 8;
    }
}

// Helper function to convert character to digit value
static int char_to_digit(char c, int base) {
    int digit;
    if (c >= '0' && c <= '9') {
        digit = c - '0';
    } else if (c >= 'a' && c <= 'f') {
        digit = c - 'a' + 10;
    } else if (c >= 'A' && c <= 'F') {
        digit = c - 'A' + 10;
    } else {
        return -1;  // Invalid digit
    }

    return (digit < base) ? digit : -1;
}

// Helper function to finalize stream state after reading
static void finalize_stream(std::istream& is) {
    if (is) {
        is.unget();
    } else {
        is.clear();  // Clear EOF if we hit end of stream
    }
}

// Implementation for signed __int128 input
std::istream& operator>>(std::istream& is, __int128& value) {
    char c;
    if (!skip_whitespace_and_get(is, c)) {
        return is;
    }

    bool negative;
    if (!parse_sign(is, c, negative, true)) {
        return is;
    }

    // Determine base and handle prefixes
    int base = determine_base_and_advance(is, c, is.flags());
    if (base == -1) {
        return is;  // Error already set
    }

    // Validate first digit
    if (char_to_digit(c, base) == -1) {
        is.unget();
        is.setstate(std::ios::failbit);
        return is;
    }

    value = 0;
    const __int128 max_positive = (static_cast<__int128>(1) << 127) - 1;
    const __int128 max_negative = -(static_cast<__int128>(1) << 127);

    // Read digits
    do {
        int digit = char_to_digit(c, base);
        if (digit == -1) {
            break;  // Not a valid digit for this base
        }

        // Check for overflow before multiplying and adding
        if (negative) {
            if (value < (max_negative + digit) / base) {
                is.setstate(std::ios::failbit);
                return is;
            }
            value = value * base - digit;
        } else {
            if (value > (max_positive - digit) / base) {
                is.setstate(std::ios::failbit);
                return is;
            }
            value = value * base + digit;
        }
    } while (is.get(c));

    finalize_stream(is);
    return is;
}

// Implementation for unsigned __int128 input
std::istream& operator>>(std::istream& is, unsigned __int128& value) {
    char c;
    if (!skip_whitespace_and_get(is, c)) {
        return is;
    }

    bool negative;
    if (!parse_sign(is, c, negative, false)) {
        return is;
    }

    // Determine base and handle prefixes
    int base = determine_base_and_advance(is, c, is.flags());
    if (base == -1) {
        return is;  // Error already set
    }

    // Validate first digit
    if (char_to_digit(c, base) == -1) {
        is.unget();
        is.setstate(std::ios::failbit);
        return is;
    }

    value = 0;
    const unsigned __int128 max_val = ~static_cast<unsigned __int128>(0);

    // Read digits
    do {
        int digit = char_to_digit(c, base);
        if (digit == -1) {
            break;  // Not a valid digit for this base
        }

        // Check for overflow before multiplying and adding
        if (value > (max_val - digit) / base) {
            is.setstate(std::ios::failbit);
            return is;
        }

        value = value * base + digit;
    } while (is.get(c));

    finalize_stream(is);
    return is;
}

} // namespace CompuBrite
