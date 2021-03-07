#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;


Mat frame; //Frames of the video being captured


vector < vector<int>> colours  //Stores value for the HSV colourspace to identify
{   //Hue minimum,Saturation minimum,Value minimum, Hue maximum,Saturation maximum, Value maximum 
	{139,155,40,179,255,181} //red
};
vector <Scalar> colourValues //Stores values for the BGR colourspace to draw
{
	{0,0,255} //red
};

//A vector to store where to draw and what colour.Stores{ X-value, Y-value , integer value of the index of Color from Color Vector}
vector<vector<int>> allpoints; 

Point contour(Mat); //Function to find and draw contour
void draw(vector<vector<int>>, vector <Scalar>); //Function to draw 

//Function to find the colours present in colorvalues vector and find the X-value and Y-value of them in each frame
void findcolour(Mat image)
{
	Mat imageHSV; //Stores the HSV colourspace of the frame
	cvtColor(image, imageHSV, COLOR_BGR2HSV);
	for (int i = 0; i < colours.size(); i++)
	{
		Scalar lower(colours[i][0], colours[i][1], colours[i][2]);
		Scalar upper(colours[i][3], colours[i][4], colours[i][5]);
		Mat frame_t; 
		inRange(imageHSV, lower, upper, frame_t);
		
		Point points = contour(frame_t);

		if (points.x != 0 && points.y != 0)
		{
			allpoints.push_back({ points.x,points.y,i });
		}
		
	}
}

Point contour(Mat image) {

	vector<vector<Point>> contourStorage;
	vector<Vec4i> hierarchy;

	findContours(image, contourStorage, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	
	vector<vector<Point>> edges(contourStorage.size());
	vector<Rect> Rectangle(contourStorage.size());

	Point points(0, 0);
	for (int i = 0; i < contourStorage.size(); i++)
	{
		int area = contourArea(contourStorage[i]);

		if (area > 100) //Filter
		{
			float perimeter = arcLength(contourStorage[i], true);
			approxPolyDP(contourStorage[i], edges[i], 0.02 * perimeter, true);
			Rectangle[i] = boundingRect(edges[i]);

			points.x = Rectangle[i].x + Rectangle[i].width / 2; //we want to draw from the edge not the center
			points.y = Rectangle[i].y;

			//drawContours(frame, edges, i, Scalar(0, 255, 0), 2);
			rectangle(frame, Rectangle[i].tl(), Rectangle[i].br(), Scalar(0, 0, 0), 2);
		}
	}
	return points; // this is the place where we will draw
}


void draw(vector<vector<int>> allpoints, vector <Scalar> colorValues)
{
	for (int i = 0; i < allpoints.size(); i++)
	{
		circle(frame, Point(allpoints[i][0], allpoints[i][1]), 5, colourValues[allpoints[i][2]], FILLED);
	}
}

void main()
{
	//FOR VIDEO capture

	VideoCapture video(0); 
	if (!video.isOpened())
	{
		cout << "Video could not be received!";
		cin.get();
		return 0;
	}

	//Reading and processing each frame
	
	while (video.read(frame))
	{
		findcolour(frame);
		draw(allpoints, colourValues);

		imshow("video",frame);

		if (waitKey(25) >= 0)
		{
			break;
		}
	}
	
	video.release();
	destroyAllWindows();

}