#include "classifier.hpp"
#include "histogram.hpp"

using namespace std;

Classifier::Classifier(Histogram& s, Histogram& ns)
{
  calculateProbabilities(s, ns);
}

Classifier::~Classifier()
{
}

cv::Mat Classifier::classifyImage(cv::Mat& yuvImage)
{
  cv::Mat probImage = cv::Mat(yuvImage.size(), CV_32FC1);
  for (register int x = 0; x < yuvImage.rows; x++)
    for (register int y = 0; y < yuvImage.cols; y++)
    {
      int u = yuvImage.at<cv::Vec3b>(x, y)[2];
      int v = yuvImage.at<cv::Vec3b>(x, y)[1];
      probImage.at<float>(x, y) = (float) probabilities[u][v];
    }
  return probImage;
}

void Classifier::calculateProbabilities(Histogram& skin, Histogram& nonSkin)
{
  unsigned long int totalSkin = skin.getTotal();
  unsigned long int totalNonSkin = nonSkin.getTotal();
  unsigned long int total = totalSkin + totalNonSkin;

  double probSkin = (double) totalSkin / total;

  double probColour, probColourGivenSkin;

  for (register int u = 0; u < skin.getRows(); u++)
    for (register int v = 0; v < skin.getCols(); v++)
    {
      probColour = (double) (skin.getValue(u, v) + nonSkin.getValue(u, v)) / total;
      probColourGivenSkin = (double) skin.getValue(u, v) / totalSkin;
      probabilities[u][v] = probColourGivenSkin * probSkin / probColour;
    }
}
