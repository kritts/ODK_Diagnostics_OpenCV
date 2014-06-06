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
	Mat croppedBlurred;

	// Blur the image 
	//GaussianBlur(cropedImage, croppedBlurred, Size(1, 1), 10.0);

	// Increase contrast 
	//equalizeHist(croppedBlurred, croppedBlurred);
	 
	// Additional threshold 
	croppedBlurred = croppedBlurred > 100;
 
	// Make the image "black and white" by examining pixels over a certain intensity only (high threshold) 
	threshold(cropedImage, croppedBlurred, // input and output 
			  50,							  // treshold value 
			  255,							  // max binary value 
			  THRESH_BINARY | THRESH_OTSU);   // required flag to perform Otsu thresholding
	
	 
	Mat output;
	int erosion_size = 3;	
	Mat element = getStructuringElement(MORPH_CROSS, Size(2 * erosion_size + 1, 2 * erosion_size + 1), Point(erosion_size, erosion_size));
	Mat dilated;
	 
	// Apply the erosion operation
	erode(croppedBlurred, croppedBlurred, element);
	dilate(croppedBlurred, croppedBlurred, element);
 
	// Finding contours
	// Thresholds
	int thresh = 50;
	int max_thresh = 255;
	RNG rng(12345);
	double area;

	Mat canny_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	vector<Point> approx;

	// Detect edges using canny
	Canny(croppedBlurred, canny_output, thresh, thresh * 2, 3);
	
	// Find contours
	findContours(canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	 
	// Draw contours
	Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);

	for (int j = 0; j < contours.size(); j++) {
		area = contourArea(contours[j]);
		approxPolyDP(contours[j], approx, 5, true);

		if (area > 300) { 
			cout << "Area of circle: ";
			cout << area;
			cout << "\n";

			Scalar color = Scalar(255, 255, 255); 

			drawContours(drawing, contours, j, Scalar(0, 255, 255), CV_FILLED);
			vector<Point>::iterator vertex;
			
			for (vertex = approx.begin(); vertex != approx.end(); ++vertex)
			{
				circle(croppedBlurred, *vertex, 3, Scalar(0, 0, 255), 1);
			}
		} 
 
	}
	cout << "\n";
	 

	 
	namedWindow("Blue Channel", CV_WINDOW_FREERATIO);
	namedWindow("Canny Output", CV_WINDOW_FREERATIO);
	namedWindow("Blue Channel + Cropped", CV_WINDOW_FREERATIO); 
	namedWindow("Original Photo - With Fiducials", CV_WINDOW_FREERATIO);
	 
	imshow("Blue Channel", blue_channel);
	imshow("Canny Output", canny_output);
	imshow("Blue Channel + Cropped", croppedBlurred);
	imshow("Original Photo - With Fiducials", drawing);
 
	waitKey(0);

	return 0;
} 
