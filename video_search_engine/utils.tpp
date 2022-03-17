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