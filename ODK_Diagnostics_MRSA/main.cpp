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


	int originalX = cols / 8;
	int originalY = rows * 3 / 8;
	int width = cols * 7 / 8 - originalX;
	int height = rows * 3 / 4 - rows * 2 / 8;


	// Roughly cropping the image 
	Mat cropedImage = blue_channel(Rect(originalX, originalY, width, height));
	Mat original_cropped = src(Rect(originalX, originalY, width, height));
	
	// Blur the image  (necessary step?)
	Mat croppedBlurred;
	GaussianBlur(cropedImage, croppedBlurred, Size(1, 1), 10.0);

	// Increase constrast 
	equalizeHist(croppedBlurred, croppedBlurred);
	 
	// Additional threshold (necessary step?)
	//croppedBlurred = croppedBlurred > 150;

	
	// Make the image "black and white" by examining pixels over a certain intensity only (high threshold) 
	threshold(croppedBlurred, croppedBlurred, // input and output 
			  100,							  // treshold value 
			  255,							  // max binary value 
			  THRESH_BINARY | THRESH_OTSU);   // required flag to perform Otsu thresholding
	
	 
	Mat output;
	int erosion_size = 3;	// for fiducials 
	Mat element = getStructuringElement(MORPH_CROSS, Size(2 * erosion_size + 1, 2 * erosion_size + 1), Point(erosion_size, erosion_size));
	Mat dilated;
	 
	// Apply the erosion operation
	erode(croppedBlurred, croppedBlurred, element);
	dilate(croppedBlurred, croppedBlurred, element);


	erosion_size = 7;		// for region outside test 
	element = getStructuringElement(MORPH_CROSS, Size(2 * erosion_size + 1, 2 * erosion_size + 1), Point(erosion_size, erosion_size));
	erode(croppedBlurred, croppedBlurred, element);
	dilate(croppedBlurred, croppedBlurred, element);
	  
	vector<Vec3f> circles;
	// Apply the Hough Transform to find the circles
	HoughCircles(croppedBlurred, circles, CV_HOUGH_GRADIENT,
				1,								// accumulator resolution (size of the image / 2)
				croppedBlurred.cols / 8,		// minimum distance between two circles
				1500,							// Canny high threshold
				10,								// minimum number of votes
				10, 25);						// min and max radius
	std::cout << "Number of circles found: ";
	std::cout << circles.size();


	int rowsCropped = original_cropped.rows;
	int colsCropped = original_cropped.cols;

	// Draw the circles detected
	for (size_t i = 0; i < circles.size(); i++)
	{
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);
		if (center.x <= colsCropped / 2 && center.y <= rowsCropped / 2)
		{
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
	}
	std::cout << "\n";
	 


	//	namedWindow("Original", CV_WINDOW_FREERATIO);
	//	namedWindow("Blue Channel", CV_WINDOW_FREERATIO);
		namedWindow("Blue Channel + Cropped", CV_WINDOW_FREERATIO); 
		namedWindow("Original Photo - With Fiducials", CV_WINDOW_FREERATIO);

	//	imshow("Original", src);
	//	imshow("Blue Channel", blue_channel);
		imshow("Blue Channel + Cropped", croppedBlurred);
		imshow("Original Photo - With Fiducials", original_cropped);

	waitKey(0);

	return 0;
}