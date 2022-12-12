/*
 * This file is part of kuloPo/video_search_engine.
 * Copyright (c) 2021-2022 Wen Kang, Alberto Krone-Martins
 *
 * kuloPo/video_search_engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * kuloPo/video_search_engine is distributed in the hope that it will be useful,but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with kuloPo/video_search_engine. If not, see <https://www.gnu.org/licenses/>.
 */

#include <iostream>

#include "similar.h"
#include "io.h"

int main() {
    read_config();
    std::vector<double> interval_sec_1 = { 1,2,3,4,2.5,2.5,6 };
    std::vector<double> interval_sec_2 = { 1,2,1.5,1.5,4,5,6 };
    int similarity = interval_comparison(interval_sec_1, interval_sec_2);
    cout << "similarity: " << similarity << endl;
    return 0;
}

/*
Should return 7
interval 1: 1 2 3       4 2.5 2.5 6
interval 2: 1 2 1.5 1.5 4 5       6
*/