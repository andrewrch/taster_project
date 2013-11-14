#include <iostream>
#include <opencv2/highgui/highgui.hpp>

#include "histogram.hpp"

using namespace std;

Histogram::Histogram(string filename) :
  total(0),
  max(0)
{
  if (!filename.empty())
  {
    cv::FileStorage fs(filename, cv::FileStorage::READ);
    fs["histogram"] >> data;
  }

  for (register int row = 0; row < data.rows; row++)
    for (register int col = 0; col < data.cols; col++)
    {
      total += data.at<float>(row, col);
      // Increment max value (Used for scaling)
      if (data.at<float>(row, col) > max)
        max = data.at<float>(row, col);
    }
}
