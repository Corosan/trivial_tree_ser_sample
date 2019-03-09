#include <fstream>
#include <iostream>
#include <sstream>

#include "tree.h"

bool test() {
    const std::string sample_tree =
        "p1\n"
        "c1 c2   c3\n"
        "c1\n"
        "  10 11 1.1\n"
        "c2\n"
        " aaa\n"
        "10\n"
        "10+1\n";

    tree t;
    std::istringstream iss(sample_tree);
    t.deserialize(iss);

    std::ostringstream oss;
    t.serialize(oss);

    const char* test =
        "p1\n"
        "c1 c2 c3\n"
        "c1\n"
        "10 11 1.1\n"
        "c2\n"
        "aaa\n"
        "10\n"
        "10+1\n";

    if (oss.str() != test) {
        std::cout << "invalid serialization/deserializaion. Expected:\n"
            << test << "\nGot:\n" << oss.str() << std::endl;
        return false;
    }

    std::cout << "tree dump:\n";
    t.print(std::cout, 1);

    return true;
}

int main() {
    return test() ? 0 : 1;
}
