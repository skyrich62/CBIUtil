/**
 * The MIT License (MIT)
 *
 * @copyright
 * Copyright (c) 2020 Rich Newman
 *
 * Google Test suite for string_record functionality
 */

#include "CompuBrite/string_record.h"
#include <gtest/gtest.h>
#include <string>

namespace cbi = CompuBrite;

class StringRecordTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup for each test
    }
    
    void TearDown() override {
        // Cleanup after each test
    }
};

TEST_F(StringRecordTest, EqualStringsHaveSameIndex) {
    std::string hello("hello");
    std::string world("world");

    auto r1 = cbi::string_record::from_string(hello);
    auto r2 = cbi::string_record::from_string(world);
    auto r3 = cbi::string_record::from_string(hello);

    // Equal strings should have same index
    EXPECT_EQ(r1.index(), r3.index());
    
    // Different strings should have different indices
    EXPECT_NE(r1.index(), r2.index());
}

TEST_F(StringRecordTest, UnequalStringsDifferentIndex) {
    std::string hello("hello");
    std::string world("world");

    auto r1 = cbi::string_record::from_string(hello);
    auto r2 = cbi::string_record::from_string(world);

    // Different strings should have different indices
    EXPECT_NE(r1.index(), r2.index());
}

TEST_F(StringRecordTest, StringLiteralCreation) {
    auto r1 = cbi::string_record::from_string("hello");
    
    // Should be able to retrieve the string
    EXPECT_EQ(r1.string(), "hello");
    
    // Index should be valid (non-negative)
    EXPECT_GE(r1.index(), 0);
}

TEST_F(StringRecordTest, MultipleStringRecords) {
    auto r1 = cbi::string_record::from_string("hello");
    auto r2 = cbi::string_record::from_string("world");
    auto r3 = cbi::string_record::from_string("baz");

    // All should have valid strings
    EXPECT_EQ(r1.string(), "hello");
    EXPECT_EQ(r2.string(), "world");
    EXPECT_EQ(r3.string(), "baz");

    // All should have different indices
    EXPECT_NE(r1.index(), r2.index());
    EXPECT_NE(r1.index(), r3.index());
    EXPECT_NE(r2.index(), r3.index());

    // All indices should be valid (non-negative)
    EXPECT_GE(r1.index(), 0);
    EXPECT_GE(r2.index(), 0);
    EXPECT_GE(r3.index(), 0);
}

TEST_F(StringRecordTest, StringDeduplication) {
    // Create multiple records with the same string
    auto r1 = cbi::string_record::from_string("duplicate");
    auto r2 = cbi::string_record::from_string("duplicate");
    auto r3 = cbi::string_record::from_string("duplicate");

    // All should have the same index (deduplication)
    EXPECT_EQ(r1.index(), r2.index());
    EXPECT_EQ(r1.index(), r3.index());
    EXPECT_EQ(r2.index(), r3.index());

    // All should have the same string content
    EXPECT_EQ(r1.string(), "duplicate");
    EXPECT_EQ(r2.string(), "duplicate");
    EXPECT_EQ(r3.string(), "duplicate");
}

TEST_F(StringRecordTest, EmptyString) {
    auto r1 = cbi::string_record::from_string("");
    auto r2 = cbi::string_record::from_string("");

    // Empty strings should be deduplicated
    EXPECT_EQ(r1.index(), r2.index());
    
    // Should return empty string
    EXPECT_EQ(r1.string(), "");
    EXPECT_EQ(r2.string(), "");
    
    // Should have valid index
    EXPECT_GE(r1.index(), 0);
}

TEST_F(StringRecordTest, VeryLongString) {
    std::string longString(1000, 'x');  // 1000 'x' characters
    
    auto r1 = cbi::string_record::from_string(longString);
    auto r2 = cbi::string_record::from_string(longString);

    // Should deduplicate long strings too
    EXPECT_EQ(r1.index(), r2.index());
    
    // Should preserve the full string
    EXPECT_EQ(r1.string(), longString);
    EXPECT_EQ(r2.string(), longString);
    EXPECT_EQ(r1.string().length(), 1000);
}

TEST_F(StringRecordTest, MixedTypes) {
    // Test with both string objects and literals
    std::string str1("test");
    const char* str2 = "test";
    
    auto r1 = cbi::string_record::from_string(str1);
    auto r2 = cbi::string_record::from_string(str2);
    
    // Should be deduplicated regardless of source type
    EXPECT_EQ(r1.index(), r2.index());
    EXPECT_EQ(r1.string(), r2.string());
}
