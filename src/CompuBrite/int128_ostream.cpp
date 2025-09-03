#include "CompuBrite/int128_ostream.h"
#include <string>
#include <algorithm>
#include <ios>

// Helper function to convert unsigned int128 to string in specified base
static std::string to_string_base(unsigned __int128 value, int base, bool uppercase = false) {
    if (value == 0) {
        return "0";
    }

    std::string result;
    const char* digits = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";

    while (value > 0) {
        result += digits[value % base];
        value /= base;
    }

    std::reverse(result.begin(), result.end());
    return result;
}

// Helper function to apply width, fill, and alignment formatting
static std::ostream& apply_formatting(std::ostream& os, const std::string& str) {
    std::streamsize width = os.width();
    if (width <= static_cast<std::streamsize>(str.length())) {
        os.width(0);  // Reset width after use
        return os << str;
    }

    char fill_char = os.fill();
    std::ios::fmtflags flags = os.flags();
    std::streamsize padding = width - str.length();

    // Reset width (per standard behavior)
    os.width(0);

    if (flags & std::ios::left) {
        // Left alignment
        os << str;
        for (std::streamsize i = 0; i < padding; ++i) {
            os << fill_char;
        }
    } else if (flags & std::ios::internal) {
        // Internal alignment - pad between sign/base and digits
        size_t sign_base_end = 0;
        if (str[0] == '-' || str[0] == '+') {
            sign_base_end = 1;
        }
        if (str.substr(sign_base_end, 2) == "0x" || str.substr(sign_base_end, 2) == "0X") {
            sign_base_end += 2;
        } else if (sign_base_end < str.length() && str[sign_base_end] == '0' && str.length() > sign_base_end + 1) {
            // Octal prefix
            sign_base_end += 1;
        }

        os << str.substr(0, sign_base_end);
        for (std::streamsize i = 0; i < padding; ++i) {
            os << fill_char;
        }
        os << str.substr(sign_base_end);
    } else {
        // Right alignment (default)
        for (std::streamsize i = 0; i < padding; ++i) {
            os << fill_char;
        }
        os << str;
    }

    return os;
}

// Implementation for signed __int128 output
std::ostream& operator<<(std::ostream& os, __int128 value) {
    std::ios::fmtflags flags = os.flags();
    std::string result;
    bool negative = false;

    // Handle zero case
    if (value == 0) {
        if (flags & std::ios::showpos) {
            result = "+0";
        } else {
            result = "0";
        }

        // Add base prefix for zero if showbase is set
        if (flags & std::ios::showbase) {
            if (flags & std::ios::hex) {
                std::string prefix = (flags & std::ios::uppercase) ? "0X" : "0x";
                result = (flags & std::ios::showpos ? "+" : "") + prefix + "0";
            } else if (flags & std::ios::oct) {
                result = (flags & std::ios::showpos ? "+0" : "0");
            }
        }

        return apply_formatting(os, result);
    }

    // Handle sign
    if (value < 0) {
        negative = true;
        // Handle the most negative value specially to avoid overflow
        if (value == static_cast<__int128>(1) << 127) {
            unsigned __int128 abs_value = static_cast<unsigned __int128>(1) << 127;

            int base = 10;
            if (flags & std::ios::hex) base = 16;
            else if (flags & std::ios::oct) base = 8;

            result = to_string_base(abs_value, base, flags & std::ios::uppercase);
            result = "-" + result;
        } else {
            value = -value;
        }
    } else if (flags & std::ios::showpos) {
        result = "+";
    }

    // Convert to appropriate base if not already handled
    if (result.empty() || (result.size() == 1 && (result[0] == '+' || result[0] == '-'))) {
        int base = 10;
        if (flags & std::ios::hex) base = 16;
        else if (flags & std::ios::oct) base = 8;

        std::string digits = to_string_base(static_cast<unsigned __int128>(value), base, flags & std::ios::uppercase);

        // Add base prefix if showbase is set
        if (flags & std::ios::showbase) {
            if (base == 16) {
                std::string prefix = (flags & std::ios::uppercase) ? "0X" : "0x";
                digits = prefix + digits;
            } else if (base == 8 && digits != "0") {
                digits = "0" + digits;
            }
        }

        if (negative) {
            result = "-" + digits;
        } else if (flags & std::ios::showpos) {
            result = "+" + digits;
        } else {
            result = digits;
        }
    }

    // Handle base prefix for showbase if not already added
    if ((flags & std::ios::showbase) && !negative && !(flags & std::ios::showpos)) {
        if (flags & std::ios::hex) {
            std::string prefix = (flags & std::ios::uppercase) ? "0X" : "0x";
            if (result.substr(0, 2) != prefix) {
                result = prefix + result;
            }
        } else if (flags & std::ios::oct && result[0] != '0' && result != "0") {
            result = "0" + result;
        }
    }

    return apply_formatting(os, result);
}

// Implementation for unsigned __int128 output
std::ostream& operator<<(std::ostream& os, unsigned __int128 value) {
    std::ios::fmtflags flags = os.flags();
    std::string result;

    // Determine base
    int base = 10;
    if (flags & std::ios::hex) base = 16;
    else if (flags & std::ios::oct) base = 8;

    // Convert to string
    result = to_string_base(value, base, flags & std::ios::uppercase);

    // Add sign for showpos (even for unsigned)
    if (flags & std::ios::showpos) {
        result = "+" + result;
    }

    // Add base prefix if showbase is set
    if (flags & std::ios::showbase) {
        if (base == 16) {
            std::string prefix = (flags & std::ios::uppercase) ? "0X" : "0x";
            if (flags & std::ios::showpos) {
                result = "+" + prefix + result.substr(1);
            } else {
                result = prefix + result;
            }
        } else if (base == 8 && result != "0" && !(result.length() > 1 && result[0] == '+' && result[1] == '0')) {
            if (flags & std::ios::showpos) {
                result = "+0" + result.substr(1);
            } else {
                result = "0" + result;
            }
        }
    }

    return apply_formatting(os, result);
}
