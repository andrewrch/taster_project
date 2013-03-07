#include <stdint.h>
#include <cmath>
#include <iostream>

#include "depthmasker.hpp"

using namespace std;

DepthMasker::DepthMasker(uint16_t t) : 
  tolerance(t)
{}

cv::Mat DepthMasker::maskImage(
    cv::Mat& depthImage, 
    cv::Mat& validPixels, 
    vector<cv::Point>& skinPixels)
{
  cv::Mat depthMask = cv::Mat::zeros(depthImage.size(), CV_8UC1);
  cv::Mat visited = cv::Mat::zeros(depthImage.size(), CV_8UC1);

  for (vector<cv::Point>::iterator it = skinPixels.begin(); it < skinPixels.end(); ++it)
  {
    depthMask.at<uchar>(it->y, it->x) = 255;
    if (!visited.at<uchar>(it->y, it->x))
      checkNeighbours( 
          depthMask, 
          validPixels, 
          depthImage, 
          visited, 
          it->y, 
          it->x, 
          depthImage.at<uint16_t>(it->y, it->x));
  }

  return depthMask;
}

void DepthMasker::checkNeighbours(
    cv::Mat& depthMask, 
    cv::Mat& validPixels, 
    cv::Mat& depthImage,
    cv::Mat& visited, 
    int row,
    int col,
    uint16_t depth)
{
  if (!visited.at<uchar>(row, col))
  {
    visited.at<uchar>(row, col) = 1;
    if (abs(depthImage.at<uint16_t>(row, col) - depth) < tolerance 
        && depthImage.at<uint16_t>(row, col) < 2000
        && depthImage.at<uint16_t>(row, col) > 0)
        //&& validPixels.at<uchar>(row, col))
    {

      depthMask.at<uint8_t>(row, col) = 255;
      // Recursively check surrounding pixels
      if (row > 0 && !visited.at<uchar>(row - 1, col))
      	checkNeighbours(depthMask, validPixels, depthImage, visited, row - 1, col, depth);
      if (col > 0 && !visited.at<uchar>(row, col - 1))
      	checkNeighbours(depthMask, validPixels, depthImage, visited, row, col - 1, depth);
      if (col < depthImage.cols - 1 && !visited.at<uchar>(row, col + 1))
      	checkNeighbours(depthMask, validPixels, depthImage, visited, row, col + 1, depth);
      if (row < depthImage.rows - 1 && !visited.at<uchar>(row + 1, col))
      	checkNeighbours(depthMask, validPixels, depthImage, visited, row + 1, col, depth);
    }
  }
}
