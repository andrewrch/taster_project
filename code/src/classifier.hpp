#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include "histogram.hpp"
#include "blob.hpp"

class Classifier
{
  public:
    Classifier(Histogram&, Histogram&);
    ~Classifier();
    cv::Mat classifyImage(cv::Mat&);
  private:
    void calculateProbabilities(Histogram&, Histogram&);
    double probabilities[256][256];
};

#endif
