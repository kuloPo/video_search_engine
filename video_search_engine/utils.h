#pragma once

#include <vector>

void interval_merge(const std::vector<int>& interval, const int fps, std::vector<int>& interval_merged);
void interval_to_sec(const std::vector<int>& interval, const int fps, std::vector<double>& interval_sec);
template <typename T> bool vector_contain(std::vector<T>& vec, const T& elem);
template <typename T> T max_3(T x, T y, T z);
template <typename T> int argmax_3(T x, T y, T z);
#include "utils.tpp"