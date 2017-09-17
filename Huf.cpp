#include <opencv2\highgui\highgui.hpp>
#include <opencv2\core\core.hpp>
#include <opencv2\core\mat.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\opencv.hpp>
#include<iostream>
#include<math.h>
#include<conio.h>
#include<string>
#include<algorithm>
#include<bitset>
#include<iterator>

using namespace std;
using namespace cv;

static int compare(const void* p1, const void* p2) {
	int* arr1 = (int*)p1;
	int* arr2 = (int*)p2;
	return arr1[0] - arr2[0];
}

int main()
{
	Mat inpimage;

	//Change the images here
	inpimage = imread("lena128.jpg", 0);
	int magicNumber = 99988567;
	Mat img;
	map<int, string> lookupmap;

	ofstream encodedFile;
	ifstream tobedecodedFile;

	inpimage.copyTo(img);

	if (!inpimage.data)
	{
		cout << " No data entered, please enter the path to an image file" << endl;
		return -1;
	}
	imshow("Input", inpimage);

	// declare histogram
	int histogram[256] = { 0 };

	// calculate the number of pixels for each intensity value
	for (int r = 0; r < img.rows; r++) {
		for (int c = 0; c < img.cols; c++) {
			histogram[img.at<uchar>(r, c)]++;
		}
	}

	//convert to 2D array with pixels and indexes
	int hist2d[256][2] = { 0 };
	int lookup[256][2] = { 0 };
	int i = 0, col = 0, row;

	for (row = 0; row < 256; ++row)
	{
			hist2d[row][0] = histogram[i]; // histogram values
			hist2d[row][1] = i;			   // index
			i++;
	}
	
	//sorting the 2d array based on the frequency
	int size = sizeof(hist2d) / sizeof(hist2d[0]);
	qsort(hist2d, size, sizeof(hist2d[0]), compare);

	// Binary numbers are inserted into map
	for (int i = 0; i < 256; i++)
	{
		string bits = bitset<8>(i).to_string();
		lookupmap.insert(pair<int, string>(hist2d[i][1], bits));
	}

	//write magic number, image rows and column values into the number
	encodedFile.open("encoded.huf", ios::binary);
	encodedFile << magicNumber << endl;
	encodedFile << img.rows << " " << img.cols << endl;
	encodedFile << '#' << endl;

	//Write the code values into the encoded file
	for (int r = 0; r < img.rows; r++) {
		for (int c = 0; c < img.cols; c++) {
			encodedFile << lookupmap[img.at<uchar>(r, c)] << " ";
		}
	}

	encodedFile.close();
	cout << "File encoded successfully";
	cout << endl;
	cout << "The encoded file is stored in encoded.huf";
	cout << endl;
	cout << "======================================================================================" << endl;
	cout << "Beginning the process of decoding . . ." << endl;

	//initialize the magic number and the delimiter
	int magicnumber = 0,imgrows, imgcols;
	char limiterchecker = NULL;

	tobedecodedFile.open("encoded.huf",ios::binary);
	tobedecodedFile >> magicnumber;

	//If there is magic number exit and return
	if (magicnumber != magicNumber)
	{
		cout << "Magic number do not match" << endl;
		return 1;
	}

	tobedecodedFile >> imgrows >> imgcols;

	cout << "image row count : " << imgrows << "image column count : " << imgcols;
	cout << endl;

	while (limiterchecker != '#')
	{
		tobedecodedFile >> limiterchecker;
	}

	cv::Mat decoded(imgrows, imgcols, CV_8UC1);

	//to read the codes from the text file
	string code;

	//vector containing the pixel values
	vector<int> pixelValues;

	//An iterator to  split the vector for number of cols the image has
	vector<int>::iterator splitIterator;

	//An iterator to get the key by the value, to retrieve the pixelvalue for every code in the file
	map<int, string>::const_iterator codereadIterator;

	//To get the key for the value
	int key = -1;

	//Read the encoded file and get the codes
	//compare the codes to the look up table and get the pixel value
	while (tobedecodedFile >> code)
	{
		for (codereadIterator = lookupmap.begin(); codereadIterator != lookupmap.end(); ++codereadIterator)
		{
			if (codereadIterator->second == code)
			{
				key = codereadIterator->first;
				pixelValues.push_back(key);
				break;
			}
		}
	}

	tobedecodedFile.close();

	//Converting the linear vector to rows*cols
	for (int r = 0; r < decoded.rows; r++)
	{ 
		i = 0;
		for (splitIterator = pixelValues.begin() + (r * decoded.cols); splitIterator != pixelValues.end(); ++splitIterator,++i)
		{
			if (i >= decoded.cols)
				break;
			decoded.at<uchar>(r, i) = *splitIterator;
		
		} 	
	}

	//Save image
	imwrite("decoded.jpg", decoded);
	imshow("Decoded", decoded);
	cout << "==========================================================================================" << endl;
	cout << "Image is successfully decoded and saved as decoded.jpg";
	cout << endl;
	cout << "Press any key to exit";

	waitKey(0);
	return 0;
}