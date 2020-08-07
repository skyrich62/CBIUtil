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
