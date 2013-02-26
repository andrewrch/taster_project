#ifndef DEPTH_MASK_H
#define DEPTH_MASK_H

#include <vector>
#include <opencv2/opencv.hpp>

class DepthMasker
{
  public:
    DepthMasker(uint16_t);
    ~DepthMasker() {};
    cv::Mat maskImage(cv::Mat&, cv::Mat&, std::vector<cv::Point>&);
  private:
    void checkNeighbours(cv::Mat&, cv::Mat&, cv::Mat&, cv::Mat&, int, int, uint16_t);
    uint16_t tolerance;
};

#endif
