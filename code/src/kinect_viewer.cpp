#include <opencv2/opencv.hpp>
#include <iostream>

#include "histogram.hpp"
#include "thresholder.hpp"
#include "classifier.hpp"

using namespace std;

int main( int argc, char* argv[] ){

  Histogram skinHist(argv[1]);
  Histogram nonSkinHist(argv[2]);

  Classifier classifier(skinHist, nonSkinHist);
  Thresholder thresholder(0.4, 0.5, 20);

  ComponentLabeller labeller;

  DepthMasker masker(1);

  // Circular structure element
  cv::Mat se = cv::Mat::zeros(cv::Size(5, 5), CV_8UC1);
  cv::circle(se, cv::Point(3, 3), 2, cv::Scalar(255), -1);

  cv::VideoCapture capture(CV_CAP_OPENNI); // or CV_CAP_OPENNI
	for(;;)
	{
    // Get the frames
    cv::Mat depthMap;
    cv::Mat bgrImage;
    cv::Mat validPixels;
    cv::Mat disp;
		capture.grab();
		capture.retrieve( depthMap, CV_CAP_OPENNI_DEPTH_MAP ); // Depth values in mm (CV_16UC1)
		capture.retrieve( bgrImage, CV_CAP_OPENNI_BGR_IMAGE );
		capture.retrieve( validPixels, CV_CAP_OPENNI_VALID_DEPTH_MASK );
		capture.retrieve( disp, CV_CAP_OPENNI_DISPARITY_MAP );

    //cv::medianBlur(yuvImage, yuvImage, 3);
    cv::GaussianBlur(bgrImage, bgrImage, cv::Size(25, 25), 1.5);

    // Do processing with them
    cv::Mat yuvImage;
    cv::cvtColor(bgrImage, yuvImage, CV_BGR2YCrCb);
    cv::Mat prob = classifier.classifyImage(yuvImage);
    cv::Mat skin = thresholder.thresholdImage(prob, depthMap, validPixels); 

    cv::morphologyEx(skin, skin, cv::MORPH_DILATE, se); //, cv::Point(-1,-1), 5);

    cv::Mat con = skin.clone();

    vector<vector<cv::Point> > contours;
    vector<cv::Vec4i> hierarchy;
    // Find contours
    cv::findContours( con, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE, cv::Point(0, 0));

    unsigned int biggestContour = 0, size = 0;
    for (unsigned int i = 0; i < contours.size(); i++)
      if (contours[i].size() > size)
      {
        biggestContour = i;
        size = contours[i].size();
      }

    // Draw contours
    cv::Mat drawing = cv::Mat::zeros( skin.size(), CV_8UC1 );
//    for( int i = 0; i< contours.size(); i++ )
    cv::drawContours( drawing, contours, biggestContour, cv::Scalar(255), CV_FILLED, 8, hierarchy, 0, cv::Point() );

    cv::Mat maskedDepth = cv::Mat::zeros(bgrImage.size(), CV_8UC1);

    for (int i = 0; i < depthMap.rows; i++)
      for (int j = 0; j < depthMap.cols; j++)
        if (drawing.at<uchar>(i, j))
          maskedDepth.at<uchar>(i, j) = disp.at<uchar>(i, j);

    cv::morphologyEx(maskedDepth, maskedDepth, cv::MORPH_CLOSE, se);

    imshow("probs", prob);
    imshow("skin", skin);
    imshow("disp", disp);
    imshow("final", maskedDepth);
    imshow("bgr", bgrImage);

		if( cv::waitKey( 30 ) >= 0 )
			break;
	}
	return 0;
}
