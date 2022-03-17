#pragma once

#include <iostream>
#include <vector>
#include <opencv2/core/cuda.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/highgui.hpp>
#include "common.h"

class Sorted_Linked_List {
public:
	struct Node {
		double distance;
		cv::cuda::GpuMat img1;
		cv::cuda::GpuMat img2;
		struct Node* next = nullptr;
	};
	int max_capacity;
	int current_num = 0;
	Node* head = nullptr;

public:
	Sorted_Linked_List(const int max_capacity);
	~Sorted_Linked_List();
	void insert(double distance, cv::cuda::GpuMat img1, cv::cuda::GpuMat img2);
	void print_list() const;
	void show_img() const;
};

void print_interval(std::vector<Key_Frame*>& key_frames, int fps, bool output_by_second = true);
template <typename T> bool vector_contain(std::vector<T>& vec, const T& elem);
template <typename T> T max_3(T x, T y, T z);
template <typename T> int argmax_3(T x, T y, T z);
#include "utils.tpp"