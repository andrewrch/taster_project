#include <vector>
#include <iostream>

#include "thresholder.hpp"

using namespace std;

Thresholder::Thresholder(float min, float max) :
  tMin(min),
  tMax(max)
{
}

cv::Mat Thresholder::thresholdImage(cv::Mat& i)
{
  vector<cv::Point> seeds = findSeeds(i);
  return findNeighbours(seeds, i);
}

vector<cv::Point> Thresholder::findSeeds(cv::Mat& image)
{
  vector<cv::Point> seeds;
  // Scan through whole image
  for (register int row = 0; row < image.rows; row++)
    for (register int col = 0; col < image.cols; col++)
    {
      // Check whether seed with classifier
      if (image.at<float>(row, col) > tMax)
        	// Add seed to vector
        	seeds.push_back(cv::Point(col, row));
    }
  return seeds;
}

cv::Mat Thresholder::findNeighbours(vector<cv::Point>& seeds, cv::Mat& image)
{
  // Keep a record of which pixels have already been looked at
  cv::Mat visited = cv::Mat::zeros(image.size(), CV_8UC1);
  cv::Mat skin = cv::Mat::zeros(image.size(), CV_8UC1);
  // For each seed, check it surrounding pixels
  for (vector<cv::Point>::iterator it = seeds.begin(); it < seeds.end(); ++it)
    if (!visited.at<uchar>(it->y, it->x))
      checkNeighbours(visited, image, skin, it->y, it->x);

  return skin;
}

// Recursive function to check neighbours of seed pixels
void Thresholder::checkNeighbours(cv::Mat& visited,
                                  cv::Mat& image,
                                  cv::Mat& skinImage,
                                  int row, int col)
{
  // Base case
  if (!visited.at<uchar>(row, col))
  {
    // Set visited to stop infinite recursion
    visited.at<uchar>(row, col) = 1;

    // Check with classifier whether it is a neighbour
    if (image.at<float>(row, col) > tMin)
    {
      // Set as skin
      skinImage.at<uint8_t>(row, col) = 255;

      // Recursively check surrounding pixels
      if (row > 0 && !visited.at<uchar>(row - 1, col))
      	checkNeighbours(visited, image, skinImage, row - 1, col);
      if (col > 0 && !visited.at<uchar>(row, col - 1))
      	checkNeighbours(visited, image, skinImage, row, col - 1);
      if (col < image.cols - 1 && !visited.at<uchar>(row, col + 1))
      	checkNeighbours(visited, image, skinImage, row, col + 1);
      if (row < image.rows - 1 && !visited.at<uchar>(row + 1, col))
      	checkNeighbours(visited, image, skinImage, row + 1, col);
    }
  }
}
