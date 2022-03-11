#include "utils.h"

Sorted_Linked_List::Sorted_Linked_List(int max_capacity) {
	this->max_capacity = max_capacity;
}

Sorted_Linked_List::~Sorted_Linked_List() {
	while (head != nullptr) {
		Node* p = head->next;
		delete head;
		head = p;
	}
}

void Sorted_Linked_List::insert(double distance, cv::cuda::GpuMat img1, cv::cuda::GpuMat img2) {
	if (current_num == max_capacity) {
		if (distance < head->distance) {
			return;
		}
	}
	if (current_num == 0) {
		Node* new_node = new Node;
		new_node->distance = distance;
		new_node->img1 = img1;
		new_node->img2 = img2;
		head = new_node;
	}
	else {
		Node* p = head;
		if (distance < p->distance) {
			Node* new_node = new Node;
			new_node->distance = distance;
			new_node->img1 = img1;
			new_node->img2 = img2;
			new_node->next = head;
			head = new_node;
		}
		else {
			while (p->next != nullptr && p->next->distance < distance) {
				p = p->next;
			}
			Node* new_node = new Node;
			new_node->distance = distance;
			new_node->img1 = img1;
			new_node->img2 = img2;
			new_node->next = p->next;
			p->next = new_node;
		}
	}
	current_num++;
	if (current_num > max_capacity) {
		Node* to_drop = head;
		head = head->next;
		delete to_drop;
		current_num--;
	}
}

void Sorted_Linked_List::print_list() const {
	Node* p = head;
	while (p != nullptr) {
		std::cout << p->distance << " ";
		p = p->next;
	}
	std::cout << std::endl;
}

void Sorted_Linked_List::show_img() const {
	Node* p = head;
	while (p != nullptr) {
		cv::Mat tmp;
		p->img1.download(tmp);
		//cv::imshow("", tmp);
		cv::waitKey(0);
		p->img2.download(tmp);
		//cv::imshow("", tmp);
		cv::waitKey(0);
		p = p->next;
	}
}

void print_interval(std::vector<Key_Frame*>& key_frames, int fps, bool output_by_second) {
	for (Key_Frame* key_frame : key_frames) {
		int interval = output_by_second ? key_frame->frame_num / fps : key_frame->frame_num;
		cout << interval << " ";
	}
	cout << endl;
}