#include <iostream>

#include "similar.h"

int main() {
    std::vector<double> interval_sec_1 = { 1,2,3 };
    std::vector<double> interval_sec_2 = { 1,2,3 };
    int similarity = interval_comparison(interval_sec_1, interval_sec_2);
    std::cout << "similarity: " << similarity << std::endl;
    return 0;
}

