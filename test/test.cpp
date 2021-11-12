#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/ximgproc/radon_transform.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <string>

#include "similar.h"

using namespace std;

int main() {
	string s1 = "abcdefgh";
	string s2 = "cde";
	std::vector<char> v1(s1.begin(), s1.end());
	std::vector<char> v2(s2.begin(), s2.end());
	
	cout << interval_comparison(v1, v2) << endl;

	return 0;
}

