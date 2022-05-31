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

#include "utils.h"

template <typename T> bool vector_contain(std::vector<T>& vec, const T& elem) {
	if (find(vec.begin(), vec.end(), elem) != vec.end())
	{
		return true;
	}
	return false;
}

template <typename T> T max_3(T x, T y, T z) {
	T max = x;
	if (y > max) {
		max = y;
	}
	if (z > max) {
		max = z;
	}
	return max;
}

template <typename T> int argmax_3(T x, T y, T z) {
	int index = 0;
	T max = x;
	if (y > max) {
		max = y;
		index = 1;
	}
	if (z > max) {
		max = z;
		index = 2;
	}
	return index;
}

template <typename T> T vector_median(std::vector<T>& v) {
	int n = v.size();
	std::nth_element(v.begin(), v.begin() + n / 2, v.end());
	return v[n / 2];
}