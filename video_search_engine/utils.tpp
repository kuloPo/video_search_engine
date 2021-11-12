#include "utils.h"

template <typename T> bool vector_contain(std::vector<T>& vec, const T& elem) {
	if (find(vec.begin(), vec.end(), elem) != vec.end())
	{
		return true;
	}
	return false;
}