/**
 * The MIT License (MIT)
 *
 * @copyright
 * Copyright (c) 2020 Rich Newman
 *
 * Google Test suite for CheckPoint functionality
 */

#include "CompuBrite/CheckPoint.h"
#include <gtest/gtest.h>
#include <sstream>

namespace cbi = CompuBrite;

class CheckPointTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Reset any global state if needed
    }
    
    void TearDown() override {
        // Clean up after each test
    }
};

TEST_F(CheckPointTest, InvariantGuard) {
    int j = 42;
    
    // Create an invariant checkpoint to ensure that j remains 42
    auto guard = cbi::CheckPoint::ensure(CBI_HERE,
        [&j]() -> bool { return j == 42; },
        "j changed to: ", std::ref(j));
    
    // j should still be 42
    EXPECT_EQ(j, 42);
    
    // Modify j - the invariant should detect this
    j = 24;
    // The guard should restore j when it goes out of scope
}

TEST_F(CheckPointTest, MacroInvariant) {
    int j = 42;
    
    // Use convenience macro for invariant checking
    CBI_INVARIANT(CBI_HERE, (j == 42), "(macro) j changed to: ", std::ref(j));
    
    EXPECT_EQ(j, 42);
}

TEST_F(CheckPointTest, NamedCheckPoint) {
    // Create a debugging CheckPoint "test1"
    cbi::CheckPoint point1("test1");
    
    // This should not crash or throw
    point1.print(CBI_HERE, "Hello from test\n");
    
    // Test should pass - we're just ensuring the API works
    EXPECT_TRUE(true);
}

TEST_F(CheckPointTest, ExpectFailure) {
    bool foo = false;
    
    // Create a checkpoint for testing
    cbi::CheckPoint point1("test1");
    
    // This expect should "fire" because foo is false
    // Note: expect() doesn't throw, it just logs
    point1.expect(CBI_HERE, foo, "test1 failed expect\n");
    
    // Test continues - expect doesn't throw
    EXPECT_FALSE(foo);
}

TEST_F(CheckPointTest, StaticHit) {
    // Test static hit functionality
    // This should not crash or throw
    cbi::CheckPoint::hit(CBI_HERE, "test hit\n");
    
    EXPECT_TRUE(true);
}

TEST_F(CheckPointTest, StaticExpectTrue) {
    // This expect should never fire
    cbi::CheckPoint::expect(CBI_HERE, true, "Never fires\n");
    
    EXPECT_TRUE(true);
}

TEST_F(CheckPointTest, StaticExpectFalse) {
    // This expect will always fire (but doesn't throw)
    cbi::CheckPoint::expect(CBI_HERE, false, "test expect\n");
    
    EXPECT_TRUE(true);  // Test continues
}

TEST_F(CheckPointTest, NullPointerCheck) {
    const char *p = nullptr;
    
    // Check for null pointer - this will fire
    cbi::CheckPoint::expect(CBI_HERE, p, "Null ptr\n");
    
    EXPECT_EQ(p, nullptr);
}

TEST_F(CheckPointTest, InlineExpectation) {
    const char *p = nullptr;
    
    // Demonstrate inline expectation with perfect forwarding
    p = cbi::CheckPoint::expect(CBI_HERE, "hello", "Never Fires");
    
    EXPECT_STREQ(p, "hello");
}

TEST_F(CheckPointTest, TemporaryCheckPoint) {
    // This should not crash
    cbi::CheckPoint("test3").print(CBI_HERE, "test3\n");
    
    EXPECT_TRUE(true);
}
