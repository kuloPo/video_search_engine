#include "similar.h"

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
	dst /= dst.rows * dst.cols;
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

	cv::Mat _radon(_row_num, _col_num, CV_32SC1);

	parallel_for_(cv::Range(0, _row_num), [&](const cv::Range& range) {
		for (int _row = range.start; _row < range.end; _row++) {
			// rotate the source by _t
			double _t = (start_angle + _row * theta);
			cv::Mat _rotated_src;
			cv::Mat _r_matrix = cv::getRotationMatrix2D(_center, _t, 1);
			cv::warpAffine(_masked_src, _rotated_src, _r_matrix, _masked_src.size());
			cv::Mat _col_mat = _radon.row(_row);
			// make projection
			cv::reduce(_rotated_src, _col_mat, 0, cv::REDUCE_SUM, CV_32SC1);
		}
	});
	cv::transpose(_radon, _radon);
	_radon.copyTo(dst);

	dst.convertTo(dst, CV_32FC1);
	dst /= dst.rows * dst.cols;
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
	// use dynamic programming to find the number of matched interval
	int n1 = v1.size();
	int n2 = v2.size();
	cv::Mat m = cv::Mat::zeros(n1 + 1, n2 + 1, CV_32SC1);
	cv::Mat path_trace = cv::Mat::zeros(n1 + 1, n2 + 1, CV_32SC2);

	for (int x = 1; x <= n1; x++) {
		for (int y = 1; y <= n2; y++) {
			if (cv::abs(v1[x - 1] - v2[y - 1]) < interval_matching_epsilon) { // difference smaller than epsilon, match found 
				m.at<int>(x, y) = m.at<int>(x - 1, y - 1) + 1; // update value
				// if this is the first match found on the same level, record its coordinate
				if (m.at<int>(x, y) > m.at<int>(x - 1, y) && m.at<int>(x, y) > m.at<int>(x, y - 1)) {
					path_trace.at<cv::Vec2i>(x, y) = cv::Vec2i(x, y);
				}
				else {
					path_trace.at<cv::Vec2i>(x, y) = _path_back_trace(m, path_trace, cv::Vec2i(x, y));
				}
			}
			else { // not match, inherit value 
				m.at<int>(x, y) = max_3(m.at<int>(x - 1, y - 1), m.at<int>(x, y - 1), m.at<int>(x - 1, y));
				path_trace.at<cv::Vec2i>(x, y) = _path_back_trace(m, path_trace, cv::Vec2i(x, y));
			}
		}
	}

	// coordinates representing the index of matched intervals
	std::vector<cv::Vec2i> paths;
	// start tracing from button right corner of mat
	cv::Vec2i last_path = path_trace.at<cv::Vec2i>(n1, n2);
	paths.push_back(cv::Vec2i(n1 + 1, n2 + 1));
	while (last_path != cv::Vec2i(0, 0)) {
		paths.push_back(last_path);
		last_path = _path_back_trace(m, path_trace, last_path);
	}

	// break intervals at where they are matched
	// sum up unmatched intervals in between and check if are equal
	// interval 1: 1 | 2 | 3       | 4 | 2.5 2.5 | 6
	// interval 2: 1 | 2 | 1.5 1.5 | 4 | 5       | 6
	std::vector<int> matched_index;

	int last_1, last_2;
	last_1 = last_2 = 0;
	while (!paths.empty()) {
		cv::Vec2i path = paths.back();
		paths.pop_back();
		int x = path[0] - 1;
		int y = path[1] - 1;
#ifdef DEBUG_INTERVAL_COMPARISON
		if (!paths.empty()) {
			cout << std::format("{}: {}, {}: {}\n", x, v1[x], y, v2[y]);
		}
#endif // DEBUG_INTERVAL_COMPARISON
		if (x - last_1 > 10 || y - last_2 > 10) {
			last_1 = x;
			last_2 = y;
			continue;
		}
		double sum_1, sum_2;
		sum_1 = sum_2 = 0;
		for (int i = last_1; i < x; i++) {
			sum_1 += v1[i];
		}
		for (int i = last_2; i < y; i++) {
			sum_2 += v2[i];
		}
		// prevent one very large interval happens to match the other
		// normal interval    : 1 2 3 4
		// very large interval: 1 ... 2 ... 3 ... 4
		if (cv::abs(sum_1 - sum_2) < interval_matching_epsilon) {
			for (int i = last_1; i <= x; i++) {
				if (i < v1.size() && (matched_index.empty() || i != matched_index.back())) {
					matched_index.push_back(i);
				}
			}
		}
		last_1 = x;
		last_2 = y;
	}

	// find consecutive sequence greater than threshold
	int sum, last, consecutive_count;
	sum = 0;
	last = -1;
	consecutive_count = 0;
	for (int i : matched_index) {
		if (last == -1) {
			consecutive_count = 1;
		}
		else if (i == last + 1) {
			consecutive_count++;
		}
		else {
			if (consecutive_count >= min_matched_interval) {
				sum += consecutive_count;
			}
			consecutive_count = 1;
		}
		last = i;
	}
	if (consecutive_count >= min_matched_interval) {
		sum += consecutive_count;
	}

#ifdef DEBUG_INTERVAL_COMPARISON
	cout << endl << endl;
	for (int i : matched_index) {
		cout << i << " ";
	}
	cout << endl << endl;

	for (int x = 1; x <= n1; x++) {
		for (int y = 1; y <= n2; y++) {
			if (m.at<int>(x, y) >= 0) {
				std::cout << m.at<int>(x, y) << " ";
			}
			else {
				std::cout << "  ";
			}
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;

	for (int x = 1; x <= n1; x++) {
		for (int y = 1; y <= n2; y++) {
			cout << std::format("({},{}) ", path_trace.at<cv::Vec2i>(x, y)[0], path_trace.at<cv::Vec2i>(x, y)[1]);
		}
		std::cout << std::endl;
	}
#endif // DEBUG_INTERVAL_COMPARISON

	return sum;
	//return matched_index.size();
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