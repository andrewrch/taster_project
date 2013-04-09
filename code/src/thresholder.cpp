#include <vector>
#include <iostream>

#include "thresholder.hpp"

using namespace std;

Thresholder::Thresholder(float min, float max, float depth) :
  tMin(min),
  tMax(max),
  tDepth(depth)
{
}

cv::Mat Thresholder::thresholdImage(cv::Mat& i, cv::Mat& depth, cv::Mat& valid, cv::Mat& prevSkin, cv::Mat& prevDepth)
{
  vector<cv::Point> seeds = findSeeds(i, depth, valid, prevSkin, prevDepth);
  return findNeighbours(seeds, i, depth, valid);
}

vector<cv::Point> Thresholder::findSeeds(
    cv::Mat& image, 
    cv::Mat& depth,
    cv::Mat& valid, 
    cv::Mat& prevSkin,
    cv::Mat& prevDepth)
{
  vector<cv::Point> seeds;
  // Scan through whole image
  for (register int row = 0; row < image.rows; row++)
    for (register int col = 0; col < image.cols; col++)
    {
      // Check whether seed with classifier
//      if (prevSkin.at<uint8_t>(row, col))
//        cout << "d: " << depth.at<uint16_t>(row, col) << " p: " << prevDepth.at<uint16_t>(row, col) << endl;
      if ( depth.at<uint16_t>(row, col) < 2000 )
        if (image.at<float>(row, col) > tMax ||
            (prevSkin.at<uint8_t>(row, col) && 
            (uabs(prevDepth.at<uint16_t>(row, col), depth.at<uint16_t>(row, col)) < tDepth)))
        	// Add seed to vector
        	seeds.push_back(cv::Point(col, row));
    }
  return seeds;
}

cv::Mat Thresholder::findNeighbours(
    vector<cv::Point>& seeds, 
    cv::Mat& image, 
    cv::Mat& depthImage,
    cv::Mat& valid)
{
  // Keep a record of which pixels have already been looked at
  cv::Mat visited = cv::Mat::zeros(image.size(), CV_8UC1);
  cv::Mat skin = cv::Mat::zeros(image.size(), CV_8UC1);

  unsigned int depth;
  // For each seed, check it surrounding pixels
  for (vector<cv::Point>::iterator it = seeds.begin(); it < seeds.end(); ++it)
    if (!visited.at<uchar>(it->y, it->x))
    {
      depth = depthImage.at<uint16_t>(it->y, it->x);
      checkNeighbours(visited, image, depthImage, valid, skin, it->y, it->x, depth);
    }

  return skin;
}

// Recursive function to check neighbours of seed pixels
void Thresholder::checkNeighbours(cv::Mat& visited,
                                  cv::Mat& image,
                                  cv::Mat& depthImage,
                                  cv::Mat& valid,
                                  cv::Mat& skinImage,
                                  int row, int col, unsigned int depth)
{
  // Base case
  if (!visited.at<uchar>(row, col))
  {
    // Set visited to stop infinite recursion
    visited.at<uchar>(row, col) = 1;

    // Check with classifier whether it is a neighbour
//    cout << depthImage.at<uint16_t>(row, col) << endl;
    if (depthImage.at<uint16_t>(row, col) < 2000)
      if (image.at<float>(row, col) > tMin || 
          (valid.at<uchar>(row, col) && uabs(depthImage.at<uint16_t>(row, col), depth) < tDepth && 
           image.at<float>(row, col) > tMin/4 ))
      {
      // Set as skin
      skinImage.at<uint8_t>(row, col) = 1;
      
      // Recursively check surrounding pixels
      if (row > 0 && !visited.at<uchar>(row - 1, col))
      	checkNeighbours(visited, image, depthImage, valid, skinImage, row - 1, col, depth);
      if (col > 0 && !visited.at<uchar>(row, col - 1))
      	checkNeighbours(visited, image, depthImage, valid, skinImage, row, col - 1, depth);
      if (col < image.cols - 1 && !visited.at<uchar>(row, col + 1))
      	checkNeighbours(visited, image, depthImage, valid, skinImage, row, col + 1, depth);
      if (row < image.rows - 1 && !visited.at<uchar>(row + 1, col))
      	checkNeighbours(visited, image, depthImage, valid, skinImage, row + 1, col, depth);
    }
  }
}
