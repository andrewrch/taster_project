#ifndef BLOB_H
#define BLOB_H

#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

class Blob
{
  public:
    Blob() : size(0) {};
    ~Blob() {};

    inline void addPoint(int x, int y)
    {
    	points.push_back(cv::Point(x, y));
      size++;
    };

    inline std::vector<cv::Point>& getPoints() {
      return points;
    };

    inline int getSize() {
      return size;
    };
  private:
    std::vector<cv::Point> points;
    int size;
};

#endif
