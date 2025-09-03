/**
 * The MIT License (MIT)
 *
 * @copyright
 * Copyright (c) 2020 Rich Newman
 *
 * Google Test suite for int128 I/O operators with iomanip support
 */

#include <sstream>
#include <iomanip>
#include <limits>

#include "CompuBrite/int128_istream.h"
#include "CompuBrite/int128_ostream.h"

// Custom printer functions for GoogleTest
// These must be defined before including gtest.h to avoid template instantiation issues
namespace testing {
namespace internal {
    
// Helper function to convert __int128 to string for printing
std::string Int128ToString(__int128 value) {
    std::ostringstream oss;
    oss << value;  // Uses our custom operator<<
    return oss.str();
}

// Helper function to convert unsigned __int128 to string for printing  
std::string UInt128ToString(unsigned __int128 value) {
    std::ostringstream oss;
    oss << value;  // Uses our custom operator<<
    return oss.str();
}

}  // namespace internal
}  // namespace testing

// Custom printer functions for GoogleTest
namespace testing {
namespace internal {

// Printer for __int128
void PrintTo(__int128 value, std::ostream* os) {
    *os << Int128ToString(value);
}

// Printer for unsigned __int128
void PrintTo(unsigned __int128 value, std::ostream* os) {
    *os << UInt128ToString(value);
}

}  // namespace internal
}  // namespace testing

// Now include gtest.h after defining our printers
#include <gtest/gtest.h>

class Int128IOTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Reset stream state for each test
    }

    void TearDown() override {
        // Cleanup after each test
    }
};

// Output Tests
TEST_F(Int128IOTest, BasicDecimalOutput) {
    std::ostringstream oss;
    __int128 value = 12345;

    oss << value;
    EXPECT_EQ(oss.str(), "12345");
}

TEST_F(Int128IOTest, NegativeDecimalOutput) {
    std::ostringstream oss;
    __int128 value = -12345;

    oss << value;
    EXPECT_EQ(oss.str(), "-12345");
}

TEST_F(Int128IOTest, ZeroOutput) {
    std::ostringstream oss;
    __int128 value = 0;

    oss << value;
    EXPECT_EQ(oss.str(), "0");
}

TEST_F(Int128IOTest, HexadecimalOutput) {
    std::ostringstream oss;
    __int128 value = 255;

    oss << std::hex << value;
    EXPECT_EQ(oss.str(), "ff");
}

TEST_F(Int128IOTest, HexadecimalUppercaseOutput) {
    std::ostringstream oss;
    __int128 value = 255;

    oss << std::hex << std::uppercase << value;
    EXPECT_EQ(oss.str(), "FF");
}

TEST_F(Int128IOTest, OctalOutput) {
    std::ostringstream oss;
    __int128 value = 64;

    oss << std::oct << value;
    EXPECT_EQ(oss.str(), "100");
}

TEST_F(Int128IOTest, ShowPositive) {
    std::ostringstream oss;
    __int128 value = 42;

    oss << std::showpos << value;
    EXPECT_EQ(oss.str(), "+42");
}

TEST_F(Int128IOTest, ShowBaseHex) {
    std::ostringstream oss;
    __int128 value = 255;

    oss << std::hex << std::showbase << value;
    EXPECT_EQ(oss.str(), "0xff");
}

TEST_F(Int128IOTest, ShowBaseOctal) {
    std::ostringstream oss;
    __int128 value = 64;

    oss << std::oct << std::showbase << value;
    EXPECT_EQ(oss.str(), "0100");
}

TEST_F(Int128IOTest, WidthAndFill) {
    std::ostringstream oss;
    __int128 value = 42;

    oss << std::setw(10) << std::setfill('*') << value;
    EXPECT_EQ(oss.str(), "********42");
}

TEST_F(Int128IOTest, LeftAlignment) {
    std::ostringstream oss;
    __int128 value = 42;

    oss << std::left << std::setw(10) << std::setfill('*') << value;
    EXPECT_EQ(oss.str(), "42********");
}

TEST_F(Int128IOTest, InternalAlignment) {
    std::ostringstream oss;
    __int128 value = -42;

    oss << std::internal << std::setw(10) << std::setfill('*') << value;
    EXPECT_EQ(oss.str(), "-*******42");
}

TEST_F(Int128IOTest, UnsignedOutput) {
    std::ostringstream oss;
    unsigned __int128 value = 12345;

    oss << value;
    EXPECT_EQ(oss.str(), "12345");
}

TEST_F(Int128IOTest, UnsignedHexOutput) {
    std::ostringstream oss;
    unsigned __int128 value = 255;

    oss << std::hex << value;
    EXPECT_EQ(oss.str(), "ff");
}

TEST_F(Int128IOTest, LargeValueOutput) {
    std::ostringstream oss;
    __int128 value = static_cast<__int128>(1) << 100;  // 2^100

    oss << value;
    // 2^100 = 1267650600228229401496703205376
    EXPECT_EQ(oss.str(), "1267650600228229401496703205376");
}

// Input Tests
TEST_F(Int128IOTest, BasicDecimalInput) {
    std::istringstream iss("12345");
    __int128 value;

    iss >> value;
    EXPECT_EQ(value, 12345);
    EXPECT_TRUE(iss.good());
}

TEST_F(Int128IOTest, NegativeDecimalInput) {
    std::istringstream iss("-12345");
    __int128 value;

    iss >> value;
    EXPECT_EQ(value, -12345);
    EXPECT_TRUE(iss.good());
}

TEST_F(Int128IOTest, HexadecimalInput) {
    std::istringstream iss("0xFF");
    __int128 value;

    iss >> value;
    EXPECT_EQ(value, 255);
    EXPECT_TRUE(iss.good());
}

TEST_F(Int128IOTest, HexadecimalInputLowercase) {
    std::istringstream iss("0xabc");
    __int128 value;

    iss >> value;
    EXPECT_EQ(value, 0xabc);
    EXPECT_TRUE(iss.good());
}

TEST_F(Int128IOTest, OctalInput) {
    std::istringstream iss("0100");
    __int128 value;

    iss >> value;
    EXPECT_EQ(value, 64);
    EXPECT_TRUE(iss.good());
}

TEST_F(Int128IOTest, HexFlagInput) {
    std::istringstream iss("ff");
    __int128 value;

    iss >> std::hex >> value;
    EXPECT_EQ(value, 255);
    EXPECT_TRUE(iss.good());
}

TEST_F(Int128IOTest, OctalFlagInput) {
    std::istringstream iss("100");
    __int128 value;

    iss >> std::oct >> value;
    EXPECT_EQ(value, 64);
    EXPECT_TRUE(iss.good());
}

TEST_F(Int128IOTest, UnsignedInput) {
    std::istringstream iss("12345");
    unsigned __int128 value;

    iss >> value;
    EXPECT_EQ(value, 12345u);
    EXPECT_TRUE(iss.good());
}

TEST_F(Int128IOTest, UnsignedNegativeInputFails) {
    std::istringstream iss("-12345");
    unsigned __int128 value = 999;  // Set to non-zero to detect change

    iss >> value;
    EXPECT_TRUE(iss.fail());
    EXPECT_EQ(value, 999u);  // Should be unchanged
}

TEST_F(Int128IOTest, WhitespaceSkipping) {
    std::istringstream iss("  \t\n  12345");
    __int128 value;

    iss >> value;
    EXPECT_EQ(value, 12345);
    EXPECT_TRUE(iss.good());
}

TEST_F(Int128IOTest, PositiveSignInput) {
    std::istringstream iss("+12345");
    __int128 value;

    iss >> value;
    EXPECT_EQ(value, 12345);
    EXPECT_TRUE(iss.good());
}

TEST_F(Int128IOTest, ZeroInput) {
    std::istringstream iss("0");
    __int128 value = 999;

    iss >> value;
    EXPECT_EQ(value, 0);
    EXPECT_TRUE(iss.good());
}

TEST_F(Int128IOTest, InvalidInput) {
    std::istringstream iss("abc");
    __int128 value = 999;

    iss >> value;
    EXPECT_TRUE(iss.fail());
    EXPECT_EQ(value, 999);  // Should be unchanged
}

TEST_F(Int128IOTest, PartialValidInput) {
    std::istringstream iss("123abc");
    __int128 value;

    iss >> value;
    EXPECT_EQ(value, 123);
    EXPECT_TRUE(iss.good());

    // Next read should get 'abc'
    std::string remaining;
    iss >> remaining;
    EXPECT_EQ(remaining, "abc");
}

// Round-trip Tests
TEST_F(Int128IOTest, RoundTripDecimal) {
    __int128 original = 123456789012345;
    std::ostringstream oss;
    std::istringstream iss;
    __int128 result;

    oss << original;
    iss.str(oss.str());
    iss >> result;

    EXPECT_EQ(original, result);
}

TEST_F(Int128IOTest, RoundTripHex) {
    __int128 original = 0x123456789ABCDEF;
    std::ostringstream oss;
    std::istringstream iss;
    __int128 result;

    oss << std::hex << std::showbase << original;
    iss.str(oss.str());
    iss >> result;

    EXPECT_EQ(original, result);
}

TEST_F(Int128IOTest, RoundTripNegative) {
    __int128 original = -123456789012345;
    std::ostringstream oss;
    std::istringstream iss;
    __int128 result;

    oss << original;
    iss.str(oss.str());
    iss >> result;

    EXPECT_EQ(original, result);
}

TEST_F(Int128IOTest, MostNegativeValue) {
    // Test the most negative __int128 value
    __int128 value = static_cast<__int128>(1) << 127;  // Most negative value
    std::ostringstream oss;

    oss << value;
    EXPECT_EQ(oss.str(), "-170141183460469231731687303715884105728");
}

TEST_F(Int128IOTest, ComplexFormattingCombination) {
    std::ostringstream oss;
    __int128 value = 42;

    oss << std::hex << std::uppercase << std::showbase
        << std::showpos << std::setw(15) << std::setfill('0')
        << std::internal << value;

    EXPECT_EQ(oss.str(), "+0X00000000002A");
}
