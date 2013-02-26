#include "Image.hpp"
#include "cv.h"
#include "highgui.h"
#include <iostream>

using namespace cv;
using namespace std;


// Creates a black 640x480 image
Image::Image(int width, int height) :
  width_(width),
  height_(height)
{
  // Create a black image
  original_image_ = Mat::zeros(Size(width_, height_), CV_8UC3);
  // Convert to YUV
  yuv_image_ = Mat::zeros(Size(width_, height_), CV_8UC3);
  final_image_ = Mat::zeros(Size(width_, height_), CV_8UC3);
  probability_image_ = Mat::zeros(Size(width_, height_), CV_32FC1);
  skin_image_ = Mat::zeros(Size(width_, height_), CV_8UC1);
  component_image_ = Mat::zeros(Size(width_, height_), CV_16UC1);
}

// Creates an image from a cv::Mat
Image::Image(Mat& img, int width, int height) :
  original_image_(img),
  width_(width),
  height_(height)
{
  // Images come from the camera in BGR for whatever reason...
  // Convert to YUV
  //cvtColor(original_image_, yuv_image_, CV_BGR2YCrCb);
  cvtColor(original_image_, original_image_, CV_BGR2RGB);

  // Now we convert the image to YUV!
  yuv_image_ = Mat(original_image_.size(), CV_8UC3);
  for (int i = 0; i < original_image_.rows; i++)
    for (int j = 0; j < original_image_.cols; j++)
    {
      // Fill in the YUV values
      Vec3b rgb = original_image_.at<Vec3b>(i, j);
      Vec3b yuv = Vec3b(((66 * rgb[0] + 129 * rgb[1] + 25 * rgb[2] + 128) >> 8) + 16 ,
                        ((-38 * rgb[0] - 74 * rgb[1] + 112 * rgb[2] + 128) >> 8) + 128 ,
                        ((112 * rgb[0] -94 * rgb[1] - 18 * rgb[2] + 128) >> 8) + 128 );
      yuv_image_.at<Vec3b>(i, j) = yuv;
    }


  // Quicker way to do this?
  Mat tmp;
  resize(yuv_image_, tmp, Size(width_, height_), 0, 0, INTER_NEAREST);
  yuv_image_ = tmp;
  //////////
  

  final_image_ = Mat::zeros(Size(width_, height_), CV_8UC3);
  // Set up the other images
  probability_image_ = Mat::zeros(Size(width_, height_), CV_32FC1);
  skin_image_ = Mat::zeros(Size(width_, height_), CV_8UC1);
  component_image_ = Mat::zeros(Size(width_, height_), CV_16UC1);
}

// Copy constructor
Image::Image(const Image& i) :
  width_(i.width_),
  height_(i.height_)
{
//  cout << "copy constructor" << endl;
  i.original_image_.copyTo(original_image_);
  i.yuv_image_.copyTo(yuv_image_);
  i.final_image_.copyTo(final_image_);
  i.probability_image_.copyTo(probability_image_);
  i.skin_image_.copyTo(skin_image_);
  i.component_image_.copyTo(component_image_);
}

// Shouldn't need to destroy anything as cv::Mats will be
// destroyed when the go out of scope.
Image::~Image()
{
}

Image& Image::operator=(const Image& i)
{
//  cout << "operator=" << endl;
  // Params
  width_ = i.width_;
  height_ = i.height_;

  // Images
  i.original_image_.copyTo(original_image_);
  i.yuv_image_.copyTo(yuv_image_);
  i.final_image_.copyTo(final_image_);
  i.probability_image_.copyTo(probability_image_);
  i.skin_image_.copyTo(skin_image_);
  i.component_image_.copyTo(component_image_);
  return *this;
}

void Image::clear_channel_at(int chan, int row, int col)
{
  Vec3b pixel  = final_image_.at<Vec3b>(row, col);
  pixel[chan] = 0;
  final_image_.at<Vec3b>(row, col) = pixel;
}
