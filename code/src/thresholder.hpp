#ifndef THRESHOLDER_H
#define THRESHOLDER_H

#include <opencv2/opencv.hpp>

class Thresholder
{
  public:
    Thresholder(float, float);
    ~Thresholder() {}
    cv::Mat thresholdImage(cv::Mat&);

  private:
    std::vector<cv::Point> findSeeds(cv::Mat&);
    cv::Mat findNeighbours(std::vector<cv::Point>&, cv::Mat&);
    void checkNeighbours(cv::Mat&, cv::Mat&, cv::Mat&, int, int);
    float tMin, tMax;
};

#endif
