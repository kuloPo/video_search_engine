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

#include "similar.h"

Interval_Comparison::Interval_Comparison(const std::vector<double>& interval1, const std::vector<double>& interval2) {
	this->interval1 = interval1;
	this->interval2 = interval2;
	this->matched1 = std::vector<Status>(interval1.size(), Status::NOT_MATCHED);
	this->matched2 = std::vector<Status>(interval2.size(), Status::NOT_MATCHED);
	this->main_p = cv::Vec2i(0, 0);
}

cv::Vec2i Interval_Comparison::find_direct_match() {
	for (int i = this->main_p[0]; i < this->interval1.size(); i++) {
		for (int j = this->main_p[1]; j < this->interval2.size(); j++) {
			if (abs(interval1[i] - interval2[j]) <= 0.5 &&
				interval1[i] >= 10 and interval2[j] >= 10) {
				return cv::Vec2i(i, j);
			}
		}
	}
	return cv::Vec2i(-1, -1);
}

bool Interval_Comparison::expand(Direction direction) {
	double sum1, sum2, increment;
	int cnt = 0;
	sum1 = sum2 = 0;
	while (abs(sum1 - sum2) > 0.5 || sum1 == 0 && sum2 == 0) {
		if (cnt > 6) {
			return false;
		}
		if (sum1 < sum2 || (sum1 == 0 && sum2 == 0)) {
			increment = this->explore(this->interval1, this->matched1, this->main_p[0], direction);
			if (increment == -1) {
				return false;
			}
			sum1 += increment;
			this->main_p[0] += direction;
		}
		else {
			increment = this->explore(this->interval2, this->matched2, this->main_p[1], direction);
			if (increment == -1) {
				return false;
			}
			sum2 += increment;
			this->main_p[1] += direction;
		}
		cnt++;
	}
	this->mark(this->matched1, this->main_p[0], direction);
	this->mark(this->matched2, this->main_p[1], direction);
	return true;
}

double Interval_Comparison::explore(
	const std::vector<double>& interval,
	const std::vector<Status>& matched,
	int p, Direction direction) const
{
	if (direction == Direction::LEFT && p == 0) {
		return -1;
	}
	if (direction == Direction::RIGHT && p == interval.size() - 1) {
		return -1;
	}
	if (matched[p + direction] != Status::NOT_MATCHED) {
		return -1;
	}
	return interval[p + direction];
}

void Interval_Comparison::mark(std::vector<Status>& matched, int p, Direction direction) {
	matched[p] = Status::EXPAND_MATCH_BOUNDARY;
	p += direction * -1;
	while (matched[p] != Status::DIRECT_MATCH && matched[p] != Status::EXPAND_MATCH_BOUNDARY) {
		matched[p] = Status::EXPAND_MATCH;
		p += direction * -1;
	}
}

void Interval_Comparison::compare() {
	this->main_p = this->find_direct_match();
	while (this->main_p != cv::Vec2i(-1, -1)) {
		this->matched1[this->main_p[0]] = Status::DIRECT_MATCH;
		this->matched2[this->main_p[1]] = Status::DIRECT_MATCH;
		while (this->expand(Direction::LEFT));
		while (this->matched1[this->main_p[0]] != Status::DIRECT_MATCH) {
			this->main_p[0]++;
		}
		while (this->matched2[this->main_p[1]] != Status::DIRECT_MATCH) {
			this->main_p[1]++;
		}
		while (this->expand(Direction::RIGHT));
		while (this->matched1[this->main_p[0]] == Status::NOT_MATCHED) {
			this->main_p[0]--;
		}
		while (this->matched2[this->main_p[1]] == Status::NOT_MATCHED) {
			this->main_p[1]--;
		}
		this->main_p[0]++;
		this->main_p[1]++;
		this->main_p = this->find_direct_match();
	}
}

int Interval_Comparison::get_matched() const {
	int sum = 0;
	for (Status i : this->matched1) {
		if (i != Status::NOT_MATCHED) {
			sum++;
		}
	}
	return sum;
}

void Interval_Comparison::print_matched() const {
	cout << "interval 1: \n";
	for (int i = 0; i < this->interval1.size(); i++) {
		double interval = this->interval1[i];
		Status status = this->matched1[i];
		printf("\x1B[%dm%.2f\033[0m ", 31 + status, interval);
	}
	cout << endl;
	cout << "interval 2: \n";
	for (int i = 0; i < this->interval2.size(); i++) {
		double interval = this->interval2[i];
		Status status = this->matched2[i];
		printf("\x1B[%dm%.2f\033[0m ", 31 + status, interval);
	}
	cout << endl;
}

#ifdef HAVE_OPENCV_CUDACODEC
cv::cuda::GpuMat get_histogram(const cv::cuda::GpuMat img) {
	cv::cuda::GpuMat hist;
	cv::cuda::calcHist(img, hist);
	hist.convertTo(hist, CV_32F);
	cv::cuda::divide(hist, cv::Scalar(img.rows * img.cols), hist);
	return hist;
}

double wasserstein_distance(const cv::cuda::GpuMat& hist1, const cv::cuda::GpuMat& hist2) {
	cv::Mat sig1, sig2;
	hist1.download(sig1);
	hist2.download(sig2);
	return cv::EMDL1(sig1, sig2);
}

int direct_distance(const cv::cuda::GpuMat& img1, const cv::cuda::GpuMat& img2) {
	cv::cuda::GpuMat tmp;
	cv::cuda::absdiff(img1, img2, tmp);
	return cv::cuda::sum(tmp)[0] / (img1.rows * img1.cols);
}

void RadonTransform(cv::cuda::GpuMat& src,
	cv::Mat& dst,
	double theta,
	double start_angle,
	double end_angle)
{
	cv::cuda::GpuMat _srcMat;
	src.copyTo(_srcMat);

	int _row_num, _col_num, _out_mat_type;
	_row_num = cvRound((end_angle - start_angle) / theta);
	cv::Point _center;

	// avoid cropping corner when rotating
	_col_num = cvCeil(sqrt(_srcMat.rows * _srcMat.rows + _srcMat.cols * _srcMat.cols));
	cv::cuda::GpuMat _masked_src(cv::Size(_col_num, _col_num), _srcMat.type(), cv::Scalar(0));
	_center = cv::Point(_masked_src.cols / 2, _masked_src.rows / 2);
	_srcMat.copyTo(_masked_src(cv::Rect(
		(_col_num - _srcMat.cols) / 2,
		(_col_num - _srcMat.rows) / 2,
		_srcMat.cols, _srcMat.rows)));

	cv::cuda::GpuMat _radon(_row_num, _col_num, CV_32SC1);

	//parallel_for_(cv::Range(0, _row_num), [&](const cv::Range& range) {
		for (int _row = 0; _row < _row_num; _row++) {
			// rotate the source by _t
			double _t = (start_angle + _row * theta);
			cv::cuda::GpuMat _rotated_src;
			cv::Mat _r_matrix = cv::getRotationMatrix2D(_center, _t, 1);
			cv::cuda::warpAffine(_masked_src, _rotated_src, _r_matrix, _masked_src.size());
			cv::cuda::GpuMat _col_mat = _radon.row(_row);
			// make projection
			cv::cuda::reduce(_rotated_src, _col_mat, 0, cv::REDUCE_SUM, CV_32SC1);
		}
	//	});
	cv::cuda::transpose(_radon, _radon);
	_radon.download(dst);

	dst.convertTo(dst, CV_32FC1);
	dst /= src.rows * src.cols;
}
#endif

double wasserstein_distance(const cv::Mat& hist1, const cv::Mat& hist2) {
	return cv::EMDL1(hist1, hist2);
}

void RadonTransform(cv::Mat& src,
	cv::Mat& dst,
	double theta,
	double start_angle,
	double end_angle)
{
	cv::Mat _srcMat;
	src.copyTo(_srcMat);

	int _row_num, _col_num, _out_mat_type;
	_row_num = cvRound((end_angle - start_angle) / theta);
	cv::Point _center;

	// avoid cropping corner when rotating
	_col_num = cvCeil(sqrt(_srcMat.rows * _srcMat.rows + _srcMat.cols * _srcMat.cols));
	cv::Mat _masked_src(cv::Size(_col_num, _col_num), _srcMat.type(), cv::Scalar(0));
	_center = cv::Point(_masked_src.cols / 2, _masked_src.rows / 2);
	_srcMat.copyTo(_masked_src(cv::Rect(
		(_col_num - _srcMat.cols) / 2,
		(_col_num - _srcMat.rows) / 2,
		_srcMat.cols, _srcMat.rows)));

	cv::Mat _radon(_row_num, _col_num, CV_32FC1);

	parallel_for_(cv::Range(0, _row_num), [&](const cv::Range& range) {
		for (int _row = range.start; _row < range.end; _row++) {
			// rotate the source by _t
			double _t = (start_angle + _row * theta);
			cv::Mat _rotated_src;
			cv::Mat _r_matrix = cv::getRotationMatrix2D(_center, _t, 1);
			cv::warpAffine(_masked_src, _rotated_src, _r_matrix, _masked_src.size());
			cv::Mat _col_mat = _radon.row(_row);
			// make projection
			cv::reduce(_rotated_src, _col_mat, 0, cv::REDUCE_SUM, CV_32FC1);
		}
	});
	cv::transpose(_radon, _radon);
	_radon.copyTo(dst);

	dst.convertTo(dst, CV_32FC1);
	dst /= src.rows * src.cols;
}

cv::Vec2i _path_back_trace(const cv::Mat& m, const cv::Mat& path_trace, const cv::Vec2i& path) {
	int x = path[0];
	int y = path[1];
	int path_index = argmax_3(m.at<int>(x - 1, y - 1), m.at<int>(x, y - 1), m.at<int>(x - 1, y));
	switch (path_index) {
	case 0:
		return path_trace.at<cv::Vec2i>(x - 1, y - 1);
	case 1:
		return path_trace.at<cv::Vec2i>(x, y - 1);
	case 2:
		return path_trace.at<cv::Vec2i>(x - 1, y);
	default:
		return cv::Vec2i(-1, -1);
	}
}

int interval_comparison(const std::vector<double>& v1, const std::vector<double>& v2) {
	Interval_Comparison comp(v1, v2);
	comp.compare();
#ifdef DEBUG_INTERVAL_COMPARISON
	comp.print_matched();
#endif
	return comp.get_matched();
}

double radon_distance(const cv::Mat& radon1, const cv::Mat& radon2) {
	std::vector<double> integral_distance;
	for (int i = 0; i < 4; i++) {
		cv::Mat integral1 = radon1.col(i);
		cv::Mat integral2 = radon2.col(i);
		integral_distance.push_back(wasserstein_distance(integral1, integral2));
	}
	return std::accumulate(integral_distance.begin(), integral_distance.end(), 0.0) / integral_distance.size();
	}