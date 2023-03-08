#pragma once
#include <opencv2/opencv.hpp>
#include <dlib/image_processing.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>


class ImageProcessor {
public:
	ImageProcessor();
	void extract_eyes_from_frame(const cv::Mat& input_frame, std::vector<cv::Mat>& output_eyes);

private:
	int detect_eyes(const cv::Mat&, std::vector<std::vector<cv::Point>>&);
	std::array<cv::Point, 4> compute_eye_rect(const cv::Point, const cv::Point, const double, const double) const;
	cv::Mat align_and_crop_eyes(const cv::Mat&, const std::array<cv::Point, 4>&) const;
	void extract_aligned_eye_rect(const cv::Mat&, std::vector<cv::Mat>&, const std::vector<cv::Point>&, int, const float = 0.05f) const;


	dlib::frontal_face_detector detector_;
	dlib::shape_predictor shape_model_;
	std::string landmark_model_path_ = "resources/shape_predictor_5_face_landmarks.dat";
	float scale_factor = 0.4f;
};
