/**
 * The MIT License (MIT)
 *
 * @copyright
 * Copyright (c) 2020 Rich Newman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * @file
 * @brief Example use of CheckPoint objects.
*/

#include "CompuBrite/CheckPoint.h"
#include "CompuBrite/ThreadPool.h"

#include <iomanip>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <future>
#include <exception>
#include <numeric>

namespace cbi = CompuBrite;

int main()
{
    // Create a debugging CheckPoint "point1"
    cbi::CheckPoint point1("test1");

    // Print a debugging message if "point1" is enabled/active
    point1.print(CBI_HERE, "Hello\n");

    bool foo = false;

    // Expect "foo" to be true, print out an exception message if not.
    // It doesn't matter if point1 is disabled, because expect doesn't
    // require an active CheckPoint object.
    point1.expect(CBI_HERE, foo, "test1 failed expect\n");

    // Create debugging CheckPoint "test2"
    cbi::CheckPoint point2("test2");

    // Print a debugging message if "point2" is active/enabled.
    point2.print(CBI_HERE, "Goodbye\n");


    // Demonstrate a CheckPoint hit.
    cbi::CheckPoint::hit(CBI_HERE, "test hit\n");

    // Demonstrate an expect which is always false, and will always "fire".
    cbi::CheckPoint::expect(CBI_HERE, false, "test expect\n");

    // Demonstrate an expect which is always true, and will never "fire"
    cbi::CheckPoint::expect(CBI_HERE, true, "Never fires\n");

    const char *p = nullptr;

    // An easy way to check for a null pointer.
    cbi::CheckPoint::expect(CBI_HERE, p, "Null ptr\n");

    // Demonstrate an inline expectation.  "hello" is not null, so this
    // doesn't fire, but since "hello" is the condition which is perfectly
    // forwarded as the return value, the variable "p" gets set to "hello"
    p = cbi::CheckPoint::expect(CBI_HERE, "hello", "Never Fires");
    std::cout << "p = " << p << std::endl;

    // Create a temporary CheckPoint and print it if it's enabled.
    cbi::CheckPoint("test3").print(CBI_HERE, "test3\n");

    cbi::ThreadPool pool;
    pool.activate(6);
    const auto sleepDuration = 4u;
    for (auto i = sleepDuration * 2u; i > 0; --i) {
        pool << [i]()
        {
            sleep(1);
            std::ostringstream os;
            os << "I'm task " << std::setw(2) << i << ", Thread: " << std::hex
               << std::this_thread::get_id() << std::endl;
            std::cout << os.str();
        };
    }

    std::vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    auto work = [first = v.begin(), last = v.end()] () -> int
    {
        std::ostringstream os;
        os << "I'm the accum task, Thread: " << std::hex
           << std::this_thread::get_id() << std::endl;
        std::cout << os.str();
        sleep(5);
        auto r = std::accumulate(first, last, 0);
        cbi::CheckPoint::hit(CBI_HERE, "r = ", r);
        return r;
    };
    auto promise = pool.addTask<int>(work);
    sleep(1);
    auto future = promise.get_future();
    for (auto i = sleepDuration; i > 0; --i) {
        std::cout << i << std::endl;
        std::chrono::milliseconds span(1000);
        if (future.wait_for(span) == std::future_status::timeout) {
            continue;
        }
        try {
            auto res = future.get();
            std::cout << "result = " << res << std::endl;
        } catch (const std::exception &e) {
            std::cout << e.what() << std::endl;
        }
    }
    sleep(3);
    pool.shutdown();
    pool.wait();
    return 0;
}
