#include <iostream>

#include "similar.h"

int min_matched_interval = 3;

int main() {
    std::vector<double> interval_sec_1 = { 1,2,3,4,2.5,2.5,6 };
    std::vector<double> interval_sec_2 = { 1,2,1.5,1.5,4,5,6 };
    int similarity = interval_comparison(interval_sec_1, interval_sec_2);
    std::cout << "similarity: " << similarity << std::endl;
    return 0;
}

/*
Should return 7
interval 1: 1 2 3       4 2.5 2.5 6
interval 2: 1 2 1.5 1.5 4 5       6
*/