/**
 * The MIT License (MIT)
 *
 * @copyright
 * Copyright (c) 2020 Rich Newman
 *
 * Google Test suite for ThreadPool functionality
 */

#include "CompuBrite/ThreadPool.h"
#include "CompuBrite/CheckPoint.h"
#include <gtest/gtest.h>
#include <iomanip>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <future>
#include <exception>
#include <numeric>
#include <sstream>
#include <chrono>
#include <thread>

namespace cbi = CompuBrite;

class ThreadPoolTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup for each test
    }
    
    void TearDown() override {
        // Cleanup after each test
    }
};

TEST_F(ThreadPoolTest, BasicActivation) {
    cbi::ThreadPool pool;
    
    // Should be able to activate with 6 threads
    EXPECT_NO_THROW(pool.activate(6));
    
    // Clean shutdown
    pool.shutdown();
    pool.wait();
}

TEST_F(ThreadPoolTest, SimpleTaskExecution) {
    cbi::ThreadPool pool;
    pool.activate(2);
    
    std::atomic<int> counter{0};
    
    // Add a simple task
    pool << [&counter]() {
        counter++;
    };
    
    // Give time for task to execute
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    pool.shutdown();
    pool.wait();
    
    EXPECT_EQ(counter.load(), 1);
}

TEST_F(ThreadPoolTest, MultipleTaskExecution) {
    cbi::ThreadPool pool;
    pool.activate(4);
    
    const int numTasks = 10;
    std::atomic<int> counter{0};
    
    // Add multiple tasks
    for (int i = 0; i < numTasks; ++i) {
        pool << [&counter, i]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            counter++;
        };
    }
    
    // Wait for all tasks to complete
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    pool.shutdown();
    pool.wait();
    
    EXPECT_EQ(counter.load(), numTasks);
}

TEST_F(ThreadPoolTest, TaskWithReturnValue) {
    cbi::ThreadPool pool;
    pool.activate(2);
    
    std::vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    
    auto work = [first = v.begin(), last = v.end()]() -> int {
        // Simulate some work
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        auto r = std::accumulate(first, last, 0);
        return r;
    };
    
    auto promise = pool.addTask<int>(work);
    auto future = promise.get_future();
    
    // Wait for result with timeout
    auto status = future.wait_for(std::chrono::seconds(2));
    EXPECT_EQ(status, std::future_status::ready);
    
    if (status == std::future_status::ready) {
        try {
            auto result = future.get();
            EXPECT_EQ(result, 45);  // Sum of 1+2+3+4+5+6+7+8+9
        } catch (const std::exception& e) {
            FAIL() << "Task threw exception: " << e.what();
        }
    } else {
        FAIL() << "Task timed out";
    }
    
    pool.shutdown();
    pool.wait();
}

TEST_F(ThreadPoolTest, ConcurrentTaskExecution) {
    cbi::ThreadPool pool;
    pool.activate(4);
    
    const int numTasks = 8;
    std::vector<std::thread::id> threadIds;
    std::mutex idMutex;
    
    // Add tasks that record their thread IDs
    for (int i = 0; i < numTasks; ++i) {
        pool << [&threadIds, &idMutex, i]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            std::lock_guard<std::mutex> lock(idMutex);
            threadIds.emplace_back(std::this_thread::get_id());
        };
    }
    
    // Wait for all tasks
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    pool.shutdown();
    pool.wait();
    
    // Should have executed all tasks
    EXPECT_EQ(threadIds.size(), numTasks);
    
    // Should have used multiple threads (with 4 thread pool)
    std::set<std::thread::id> uniqueIds(threadIds.begin(), threadIds.end());
    EXPECT_GT(uniqueIds.size(), 1u);  // Should use more than 1 thread
    EXPECT_LE(uniqueIds.size(), 4u);  // But no more than 4 (pool size)
}

TEST_F(ThreadPoolTest, ShutdownAndWait) {
    cbi::ThreadPool pool;
    pool.activate(2);
    
    std::atomic<bool> taskStarted{false};
    std::atomic<bool> taskCompleted{false};
    
    // Add a longer-running task
    pool << [&taskStarted, &taskCompleted]() {
        taskStarted = true;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        taskCompleted = true;
    };
    
    // Wait for task to start
    while (!taskStarted) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    // Shutdown and wait - this should complete the running task
    pool.shutdown();
    pool.wait();
    
    // Task should have completed
    EXPECT_TRUE(taskCompleted.load());
}
