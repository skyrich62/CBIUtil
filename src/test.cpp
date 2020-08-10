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
*/

#include "compubrite/checkpoint.h"

namespace cbi = CompuBrite;

int main()
{
    cbi::CheckPoint point1("test1");
    point1.print(CBI_HERE, "Hello\n");

    point1.expect(CBI_HERE, false, "test1 failed expect\n");

    cbi::CheckPoint point2("test2");
    point2.print(CBI_HERE, "Goodbye\n");


    cbi::CheckPoint::hit(CBI_HERE, "test hit\n");
    cbi::CheckPoint::expect(CBI_HERE, false, "test expect\n");
    cbi::CheckPoint::expect(CBI_HERE, true, "Never fires\n");

    const char *p = nullptr;
    cbi::CheckPoint::expect(CBI_HERE, p, "Null ptr\n");

    p = cbi::CheckPoint::expect(CBI_HERE, "hello", "Never Fires");
    std::cout << "p = " << p << std::endl;

    cbi::CheckPoint("test3").print(CBI_HERE, "test3\n");
    return 0;
}
