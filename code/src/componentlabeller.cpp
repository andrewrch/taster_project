#include <vector>
#include <stdint.h>

#include "componentlabeller.hpp"
#include "disjointset.hpp"

using namespace std;

vector<Blob> ComponentLabeller::createBlobVector(cv::Mat& labels)
{
  vector<Blob> blobs;
  // Vector of blobs
  uint16_t numBlobs = 512;
  unsigned int currentBlobIndex = 0;
  int* blobIndexes = new int[numBlobs];
  for (int i = 0; i < numBlobs; i++)
    blobIndexes[i] = -1;

  uint16_t tempLabel, tempBlobSize;
  for (register int row = 0; row < labels.rows; row++)
    for (register int col = 0; col < labels.cols; col++)
      // Check whether pixel is skin
      if (labels.at<uint16_t>(row, col))
      {
        tempLabel = labels.at<uint16_t>(row, col);
        if (tempLabel >= numBlobs)
        {
          tempBlobSize = numBlobs;
          numBlobs = tempLabel * 2;
          int* temp = new int[numBlobs];
          for (uint16_t i = 0; i < numBlobs; i++)
            temp[i] = -1;
          for (uint16_t i = 0; i < tempBlobSize; i++)
            temp[i] = blobIndexes[i];
          delete[] blobIndexes;
          blobIndexes = temp;
        }

        if (blobIndexes[tempLabel] == -1)
        {
          blobs.push_back(Blob());
          blobIndexes[tempLabel] = currentBlobIndex;
          currentBlobIndex++;
        }
      	blobs[blobIndexes[tempLabel]].addPoint(col, row);
      }

  delete[] blobIndexes;
  return blobs;
}

cv::Mat ComponentLabeller::connectedComponentLabelling(cv::Mat& skin)
{
  // Disjoint sets for labels
  DisjointSet sets;
  unsigned int currentLabel = 1;

  // Array to hold labels
  // cols = y axis
  // rows = x axis
  unsigned int labelMatrix[skin.rows][skin.cols];

  for (register int i = 0; i < skin.rows; i++)
    for (register int j = 0; j < skin.cols; j++)
      labelMatrix[i][j] = 0;

  // Scan through whole image
  unsigned int currentValue, north_value, westValue, northLabel, westLabel;
  unsigned int* currentPixelLabel;
  for (register int row = 0; row < skin.rows; row++)
    for (register int col = 0; col < skin.cols; col++)
    {
      currentValue = skin.at<uchar>(row, col);
      // If it's a skin_ pixel
      if (currentValue)
      {
	      currentPixelLabel = &labelMatrix[row][col];
      	// Handle edge pixels by saying they are not
      	// skin_ and their labels are therefore 0
      	if (row == 0)
      	  north_value = northLabel = 0;
      	else
      	{
      	  north_value = skin.at<uchar>(row - 1, col);
      	  northLabel = labelMatrix[row - 1][col];
      	}
        
        if (col == 0)
          westValue = westLabel = 0;
        else
        {
          westValue = skin.at<uchar>(row, col - 1);
          westLabel = labelMatrix[row][col-1];
        }
        // If pixel to the west is skin_ then use that label
        if (westValue)
        {
          *currentPixelLabel = westLabel;
        }
        // If pixel to the west and north are skin_, and labels are different
        // Then they are connected by the current pixel, so should share
        // the same label
        if (north_value == westValue && northLabel != westLabel)
        {
          // Set label as smallest of north/west
          if (westLabel < northLabel)
            *currentPixelLabel = westLabel;
          else
            *currentPixelLabel = northLabel;
        
          sets.setUnion(westLabel, northLabel);
        }
        // If west is not skin_ and north is skin_
        // Then take label from north
        if (westValue != currentValue && north_value == currentValue)
        {
          *currentPixelLabel = northLabel;
        }
        // If west is not skin_ and north is not skin_
        // create new label
        if (north_value != currentValue && westValue != currentValue)
        {
          *currentPixelLabel = currentLabel;
          sets.makeSet(currentLabel);
          currentLabel++;
        }
      }
    }

  // Get reference to the label image
  cv::Mat labels = cv::Mat::zeros(skin.size(), CV_16UC1);

  // Second pass.  Set labels to smallest equivalent set
  for (register int row = 0; row < skin.rows; row++)
    for (register int col = 0; col < skin.cols; col++)
    {
      if (skin.at<uchar>(row, col))
      	labels.at<uint16_t>(row, col) = sets.find(labelMatrix[row][col]);
    }

  return labels;
}
