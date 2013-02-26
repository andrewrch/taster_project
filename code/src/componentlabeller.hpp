#ifndef COMPONENT_LABELLER_H
#define COMPONENT_LABELLER_H

#include <vector>
#include <opencv2/opencv.hpp>

#include "blob.hpp"

class ComponentLabeller
{
  public:
    ComponentLabeller() {};
    ~ComponentLabeller() {};
    std::vector<Blob> createBlobVector(cv::Mat&);
    cv::Mat connectedComponentLabelling(cv::Mat&);
  private:
};

#endif
