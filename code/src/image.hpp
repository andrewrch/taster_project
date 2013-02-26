#ifndef IMAGE_H
#define IMAGE_H

#include "cv.h"
#include "stdint.h"
#include "blob.hpp"
#include <vector>

class Image
{
  public:
    Image(int width = 640, int height = 480);
    Image(cv::Mat&, int width = 640, int height = 480);
    Image(const Image&);
    ~Image();
    Image& operator=(const Image&);

    // Getters for individual images
    inline cv::Mat& get_original_image() {
      return original_image_;
    };
    inline cv::Mat& get_yuv_image() {
      return yuv_image_;
    };
    inline cv::Mat& get_probability_image() {
      return probability_image_;
    };
    inline cv::Mat& get_skin_image() {
      return skin_image_;
    };
    inline cv::Mat& get_component_image() {
      return component_image_;
    };
    inline cv::Mat& get_final_image() {
      return final_image_;
    };

    inline unsigned int get_y_at(int row, int col)
    {
      return yuv_image_.at<cv::Vec3b>(row, col)[0];
    };
    inline unsigned int get_u_at(int row, int col)
    {
      return yuv_image_.at<cv::Vec3b>(row, col)[1];
    };
    inline unsigned int get_v_at(int row, int col)
    {
      return yuv_image_.at<cv::Vec3b>(row, col)[2];
    };
    // Methods to get some image statistics
    // Returns dimensions of the original image
    inline int get_original_rows() {
      return original_image_.rows;
    };
    inline int get_original_cols() {
      return original_image_.cols;
    };

    // Returns dimensions of the image we are processing
    inline int get_skin_rows() {
      return height_;
    };
    inline int get_skin_cols() {
      return width_;
    };

    inline int get_step() {
      return original_image_.step;
    };
    // Getter for the raw image data
    inline uint8_t* get_data() {
      return original_image_.data;
    };
  private:
    // Note:
    // The following probably seems wasteful, but I want to keep each step
    // of the processing chain, so they can each be displayed seperately for
    // testing.
    //
    // The original image from the input
    cv::Mat original_image_;
    // The image when converted to YUV
    cv::Mat yuv_image_;
    // The image after probabilities have been calculated for each pixel
    cv::Mat probability_image_;
    // The image after thresholding probabilities
    cv::Mat skin_image_;
    // The image after connected components have been generated
    cv::Mat component_image_;
    // The final final image.  Possibly dont need this...
    cv::Mat final_image_;

    int width_, height_;
};

#endif
