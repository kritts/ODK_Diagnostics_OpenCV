// Written by Krittika D'Silva (kdsilva@uw.ed)
// Code to process MRSA test strips using OpenCV 

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <opencv2/imgproc/imgproc.hpp> 
#include <stdio.h> 


using namespace cv;
using namespace std;



int main(int argc, char** argv) 
{ 
	// Check for correct number of inputs
	if (argc != 2)
	{			
		cout << " Usage: OpenCVTestCode ImageToLoadAndDisplay.jpg" << endl;
		return -1;
	}

	// Read the file
	Mat image = imread(argv[1], 1);						

	// Check for invalid input
	if (!image.data)
	{									
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}


	// Given image
	Mat src = image;
	Mat channel[3];

	// Blue channel of image 
	split(src, channel);
	Mat blue_channel = channel[0];

	int rows = blue_channel.rows;
	int cols = blue_channel.cols;

	// Roughly cropping the image 
	Mat cropedImage = blue_channel(Rect(cols / 8, rows * 3 / 8, cols * 7 / 8 - cols / 8, rows * 3 / 4 - rows * 3 / 8));
	Mat original_cropped = src(Rect(cols / 8, rows * 3 / 8, cols * 7 / 8 - cols / 8, rows * 3 / 4 - rows * 3 / 8));

	// Blur the image  (necessary step?)
	Mat croppedBlurred;
	GaussianBlur(cropedImage, croppedBlurred, Size(5, 5), 10.0);

	// Make the image "black and white" by examining pixels over a certain intensity only (high threshold) 
	croppedBlurred = croppedBlurred > 150;
	 
	Mat output;
	int erosion_size = 3;
	Mat element = getStructuringElement(MORPH_CROSS, Size(2 * erosion_size + 1, 2 * erosion_size + 1), Point(erosion_size, erosion_size));
	Mat dilated;


	// Apply the erosion operation
		erode(croppedBlurred, croppedBlurred, element);
	 	dilate(croppedBlurred, croppedBlurred, element);

	vector<Vec3f> circles;
	// Apply the Hough Transform to find the circles
	HoughCircles(croppedBlurred, circles, CV_HOUGH_GRADIENT,
		1,								// accumulator resolution (size of the image / 2)
		croppedBlurred.cols / 4,		// minimum distance between two circles
		1500,							// Canny high threshold
		10,								// minimum number of votes
		10, 20);						// min and max radius
	std::cout << "Number of circles found: ";
	std::cout << circles.size();

	// Draw the circles detected
	for (size_t i = 0; i < circles.size(); i++) 
	{
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);
		// circle center
		circle(original_cropped, center, 3, Scalar(94, 206, 165), -1, 8, 0);
		// circle outline
		circle(original_cropped, center, radius, Scalar(94, 206, 165), 3, 8, 0);

		std::cout << "\n";
		std::cout << i;
		std::cout << ": x-position: ";
		std::cout << cvRound(circles[i][0]);
		std::cout << "\t";
		std::cout << "y-position: ";
		std::cout << cvRound(circles[i][1]);
	}

	//	Mat original_cropped2 = src(Rect(cols / 8, rows * 3 / 8, cols * 7 / 8 - cols / 8, rows * 3 / 4 - rows * 3 / 8));



	//	namedWindow("Original", CV_WINDOW_FREERATIO);
	// 	namedWindow("Blue Channel", CV_WINDOW_FREERATIO);
	//	namedWindow("Blue Channel + Cropped", CV_WINDOW_FREERATIO); 
	namedWindow("Original Photo - With Fiducials", CV_WINDOW_FREERATIO);

	//	imshow("Original", src);
	//	imshow("Blue Channel", blue_channel);
	// 	imshow("Blue Channel + Cropped", blue_channel);
	imshow("Original Photo - With Fiducials", original_cropped);

	waitKey(0);

	return 0;
}