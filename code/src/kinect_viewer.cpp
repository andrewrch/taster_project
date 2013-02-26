#include <opencv2/opencv.hpp>
#include <iostream>

#include "histogram.hpp"
#include "thresholder.hpp"
#include "classifier.hpp"
#include "componentlabeller.hpp"
#include "depthmasker.hpp"

using namespace std;

int main( int argc, char* argv[] ){

  Histogram skinHist(argv[1]);
  Histogram nonSkinHist(argv[2]);

  Classifier classifier(skinHist, nonSkinHist);
  Thresholder thresholder(0.4, 0.5);

  ComponentLabeller labeller;

  DepthMasker masker(50);

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
		capture.grab();
		capture.retrieve( depthMap, CV_CAP_OPENNI_DEPTH_MAP ); // Depth values in mm (CV_16UC1)
		capture.retrieve( bgrImage, CV_CAP_OPENNI_BGR_IMAGE );
		capture.retrieve( validPixels, CV_CAP_OPENNI_VALID_DEPTH_MASK );

    // Do processing with them
    cv::Mat yuvImage;
    cv::cvtColor(bgrImage, yuvImage, CV_BGR2YCrCb);
    //cv::medianBlur(yuvImage, yuvImage, 3);
    cv::Mat prob = classifier.classifyImage(yuvImage);
    cv::Mat skin = thresholder.thresholdImage(prob); 

    cv::morphologyEx(skin, skin, cv::MORPH_CLOSE, se);

    cv::Mat labels = labeller.connectedComponentLabelling(skin);
    vector<Blob> blobs = labeller.createBlobVector(labels);

    // Get largest blob
    int maxBlobSize = 0;
    Blob* maxBlob;
    for (vector<Blob>::iterator it = blobs.begin(); it < blobs.end(); ++it)
      if (it->getSize() > maxBlobSize)
      {
        maxBlobSize = it->getSize();
        maxBlob = &(*it);
      }

    cv::Mat  theBlob = cv::Mat::zeros(bgrImage.size(), CV_8UC1);
    vector<cv::Point>& pts = maxBlob->getPoints();
    for (vector<cv::Point>::iterator it = pts.begin(); it < pts.end(); ++it)
      theBlob.at<char>(it->y, it->x) = 255;

    cv::Mat finalDepthImage = cv::Mat::zeros(bgrImage.size(), CV_8UC1);

    cv::Mat depthMask = masker.maskImage(depthMap, validPixels, maxBlob->getPoints());
    cv::morphologyEx(depthMask, depthMask, cv::MORPH_CLOSE, se);

    cv::Mat maskedDepth = cv::Mat::zeros(bgrImage.size(), CV_8UC1);

    for (int i = 0; i < depthMap.rows; i++)
      for (int j = 0; j < depthMap.cols; j++)
        if (depthMask.at<uchar>(i,j))
          maskedDepth.at<uchar>(i, j) = depthMap.at<uint16_t>(i, j);

    //cout << "rows: " << depthMap.rows << " cols: " << depthMap.cols << endl;
    //cout << "depth: " << depthMap.at<unsigned short>(160,120) << endl;

//		imshow("Depth image", depthMap);
		imshow("rgb image", bgrImage);
    imshow("Probabilities", prob);
//    imshow("Skin", skin);
    imshow("Hand", depthMask);
//    imshow("valid", validPixels);
    imshow("final", maskedDepth);

		if( cv::waitKey( 30 ) >= 0 )
			break;
	}
	return 0;
}
