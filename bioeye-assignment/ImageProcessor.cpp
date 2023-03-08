#include "ImageProcessor.h"
#include <opencv2/opencv.hpp>
#include <dlib/opencv.h>
#include <dlib/image_io.h>
#include <dlib/image_processing.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>


ImageProcessor::ImageProcessor() {
	detector_ = dlib::get_frontal_face_detector();
	dlib::deserialize(landmark_model_path_) >> shape_model_;

}


void ImageProcessor::extract_eyes_from_frame(const cv::Mat& input_frame, std::vector<cv::Mat>& output_eyes)
{
	//

	std::vector<std::vector<cv::Point>> eyes;
	if (detect_eyes(input_frame, eyes) == 1)
		return;

	output_eyes.clear();

	for (const auto eye : eyes) {
		extract_aligned_eye_rect(input_frame, output_eyes, eye, 1);
	}


}

/* detect face landmarks using dlib 5 landmarks model
*
* dlib 5 facial landmarks legend:
*
* eyes[0] --> left
* eyes[1] --> right
* eyes[i][0] --> exterior point
* eyes[i][1] -- interior point
*/

int ImageProcessor::detect_eyes(const cv::Mat& frame, std::vector<std::vector<cv::Point>>& eyes)
{
	// convert cv::Mat image to gray and to type dlib::cv_image
	cv::Mat gray_frame;
	cvtColor(frame, gray_frame, cv::COLOR_BGR2GRAY);


	cv::Mat resized;
	cv::resize(gray_frame, resized, cv::Size(0, 0), scale_factor, scale_factor);

	const dlib::cv_image<unsigned char> dlib_image_resized(resized), dlib_image(gray_frame);

	// detect faces in frame using dlib face detector
	const std::vector<dlib::rectangle> faces = detector_(dlib_image_resized);

	if (faces.empty())
		return 1;


	const dlib::rectangle face(
		faces[0].left() / scale_factor,
		faces[0].top() / scale_factor,
		faces[0].right() / scale_factor,
		faces[0].bottom() / scale_factor
	);

	cv::Point tl(face.left(), face.top()), br(face.right(), face.bottom());
	cv::rectangle(frame, tl,br, cv::Scalar(255, 0, 255));
	dlib::full_object_detection shape = shape_model_(dlib_image, face);


	for (int i = 0; i <= 2; i += 2) {
		// arranging the coordinates in a vector<Point> for each eye
		eyes.push_back(
			std::vector<cv::Point>(
				{
					cv::Point(shape.part(i).x(), shape.part(i).y()),
					cv::Point(shape.part(i + 1).x(), shape.part(i + 1).y())
				}
		));
	}

	return 0;
}
void ImageProcessor::extract_aligned_eye_rect(const cv::Mat& frame, std::vector<cv::Mat>& output_eyes, const std::vector<cv::Point>& single_eye_coordinates, int idx, const float inflate_rectangle_value) const
{
	// To align the rectangle with the eye, we will do some math
	// find the line between the two eye points --> l : y = mx + c
	// first we will find the gradient m
	const double m = static_cast<double>(single_eye_coordinates[1].y - single_eye_coordinates[0].y) / (single_eye_coordinates[1].x - single_eye_coordinates[0].x);

	// now to find the intercept c
	const double c = single_eye_coordinates[0].y - (m * single_eye_coordinates[0].x);

	// find the distance between the two eye points and calculate dx to inflate the square
	double distance = sqrt(pow(single_eye_coordinates[1].x - single_eye_coordinates[0].x, 2) + pow(single_eye_coordinates[1].y - single_eye_coordinates[0].y, 2));
	const double dx = distance * inflate_rectangle_value;


	// find which point is right and which is left from both eye points
	cv::Point right_point = (single_eye_coordinates[0].x < single_eye_coordinates[1].x) ? single_eye_coordinates[1] : single_eye_coordinates[0];
	cv::Point left_point = (single_eye_coordinates[0].x < single_eye_coordinates[1].x) ? single_eye_coordinates[0] : single_eye_coordinates[1];

	// apply the dx to each point on the same line, and update the distance
	left_point = cv::Point(left_point.x - dx, (m * (left_point.x - dx)) + c);
	right_point = cv::Point(right_point.x + dx, (m * (right_point.x + dx)) + c);
	distance = sqrt(pow(left_point.x - right_point.x, 2) + pow(left_point.y - right_point.y, 2));

	// to calculate the perpendicular lines we need the perpendicular gradient n

	const double n = -1 / m;

	// compute the eye square with distance param as width and height
	const std::array<cv::Point, 4> rect_points = compute_eye_rect(left_point, right_point, distance, n);
	//std::cout << "eyes rect computed at " << t.peek() << std::endl;

	// crop the eye rectangle from the frame
	const cv::Mat cropped_image = align_and_crop_eyes(frame, rect_points);
	//std::cout << "aligned and cropped at " << t.peek() << std::endl;

	// draw rectangle in frame
	cv::line(frame, rect_points[0], rect_points[1], cv::Scalar(255, 0, 255), 1);
	cv::line(frame, rect_points[3], rect_points[1], cv::Scalar(255, 0, 255), 1);
	cv::line(frame, rect_points[2], rect_points[0], cv::Scalar(255, 0, 255), 1);
	cv::line(frame, rect_points[2], rect_points[3], cv::Scalar(255, 0, 255), 1);

	output_eyes.push_back(cropped_image);
}
/***
 * Computes 4 points of the eye square.
 *
 * @param left_point detected by dlib.
 * @param right_point detected by dlib.
 * @param distance width of the square.
 * @param gradient of the perpendicular line to the eyes.
 * @return vector with 4 points of the square.
 */
std::array<cv::Point, 4> ImageProcessor::compute_eye_rect(
	const cv::Point left_point, const cv::Point right_point,
	const double distance, const double gradient) const
{
	cv::Point delta;
	const double r = distance / 2;

	// if origin gradient was 0, the perpendicular gradient will be inf
	// so there are 2 cases to be handled
	if (isfinite(gradient))
	{
		// if gradient is finite, this is the delta to be added
		const double denominator = sqrt(1 + pow(gradient, 2));
		delta = cv::Point(r / denominator, gradient * r / denominator);
	}
	else
	{
		// if gradient is not finite, delta is the distance/2 on y axis.
		delta = cv::Point(0, r);
	}

	cv::Point p1 = left_point - delta;
	cv::Point p2 = left_point + delta;
	cv::Point p3 = right_point - delta;
	cv::Point p4 = right_point + delta;
	//std::cout << "p1: "<< p1 << ",p2: " << p2 << ",p3: " << p3 << ",p4: " << p4 << std::endl;

	return { p1,p2,p3,p4 };
}

/***
 * Align and the crop the rectangle from original frame.
 *
 * @param frame src frame.
 * @param rect_points vector of 4 points representing eye rect.

 * @return aligned eye Mat.
 */
cv::Mat ImageProcessor::align_and_crop_eyes(const cv::Mat& frame, const std::array<cv::Point, 4>& rect_points) const
{
	// retrieve the minimum bounding rectangle around the eye
	const cv::RotatedRect rect = minAreaRect(rect_points);

	// get the rotation matrix according to the minimum bounding rectangle angle
	cv::Mat rotation_matrix;
	if (rect.angle > 45)
	{
		// fix alignment by 90 degrees for left tilt
		rotation_matrix = getRotationMatrix2D(rect.center, rect.angle - 90, 1.0);
	}
	else
	{
		rotation_matrix = getRotationMatrix2D(rect.center, rect.angle, 1.0);
	}

	// apply the rotation
	cv::Mat rotated_image;
	warpAffine(frame, rotated_image, rotation_matrix, frame.size(), cv::INTER_CUBIC);


	// retrieve the aligned bounding box from the rotated image
	const cv::Rect bounding_rect = rect.boundingRect();

	// perform the crop
	cv::Mat cropped_image;
	getRectSubPix(rotated_image, bounding_rect.size(), rect.center, cropped_image);

	return cropped_image;
}
