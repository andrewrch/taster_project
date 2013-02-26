#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <opencv2/opencv.hpp>

class Histogram {
  public:
    ~Histogram() {};
    // Default to 8bit colour
    Histogram(std::string);

    inline int getValue(int row, int col) {
      return (int) data.at<float>(row, col);
    };

    inline unsigned long int getTotal() {
      return total;
    };

    inline int getMax() {
      return max;
    }

    inline int getCols() {
      return data.cols;
    };
    inline int getRows() {
      return data.rows;
    };
    inline cv::Mat& getData() {
      return data;
    };
  protected:
    cv::Mat data;
    unsigned long int total;
    int max;
};

#endif
