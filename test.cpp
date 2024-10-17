#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>

int8_t
update_and_diff_closest_palette_color (cv::Mat& img, int i, int j) {
  auto current = img.at<uchar> (i, j);
  auto update  = (current > 127 ? 255 : 0);

  img.at<uchar> (i, j) = update;
  return int8_t (current) - int8_t (update);
}

using kernel_t = void (*) (cv::Mat&, int, int, double);

kernel_t kernel_floyd_steinberg = [] (cv::Mat& img, int i, int j, double diff) {
  auto error_diffused = diff / 16.;

  img.at<uchar> (i, j + 1)     = img.at<uchar> (i, j + 1) + error_diffused * 7.;
  img.at<uchar> (i + 1, j - 1) = img.at<uchar> (i + 1, j - 1) + error_diffused * 3.;
  img.at<uchar> (i + 1, j)     = img.at<uchar> (i + 1, j) + error_diffused * 5.;
  img.at<uchar> (i + 1, j + 1) = img.at<uchar> (i + 1, j + 1) + error_diffused;
};

kernel_t kernel_atkinson = [] (cv::Mat& img, int i, int j, double diff) {
  auto error_diffused = diff / 8.;

  img.at<uchar> (i, j + 1) = img.at<uchar> (i, j + 1) + error_diffused;
  img.at<uchar> (i, j + 2) = img.at<uchar> (i, j + 2) + error_diffused;

  img.at<uchar> (i + 1, j - 1) = img.at<uchar> (i + 1, j - 1) + error_diffused;
  img.at<uchar> (i + 1, j)     = img.at<uchar> (i + 1, j) + error_diffused;
  img.at<uchar> (i + 1, j + 1) = img.at<uchar> (i + 1, j + 1) + error_diffused;

  img.at<uchar> (i + 2, j) = img.at<uchar> (i + 2, j) + error_diffused;
};

kernel_t kernel_min_average_error = [] (cv::Mat& img, int i, int j, double diff) {
  auto error_diffused = diff / 48.;

  img.at<uchar> (i, j + 1) = img.at<uchar> (i, j + 1) + error_diffused * 7.;
  img.at<uchar> (i, j + 2) = img.at<uchar> (i, j + 2) + error_diffused * 5.;

  img.at<uchar> (i + 1, j - 2) = img.at<uchar> (i + 1, j - 2) + error_diffused * 3.;
  img.at<uchar> (i + 1, j - 1) = img.at<uchar> (i + 1, j - 1) + error_diffused * 5.;
  img.at<uchar> (i + 1, j)     = img.at<uchar> (i + 1, j) + error_diffused * 7.;
  img.at<uchar> (i + 1, j + 1) = img.at<uchar> (i + 1, j + 1) + error_diffused * 5.;
  img.at<uchar> (i + 1, j + 2) = img.at<uchar> (i + 1, j + 2) + error_diffused * 3.;

  img.at<uchar> (i + 2, j - 2) = img.at<uchar> (i + 2, j - 2) + error_diffused * 1.;
  img.at<uchar> (i + 2, j - 1) = img.at<uchar> (i + 2, j - 1) + error_diffused * 3.;
  img.at<uchar> (i + 2, j)     = img.at<uchar> (i + 2, j) + error_diffused * 5.;
  img.at<uchar> (i + 2, j + 1) = img.at<uchar> (i + 2, j + 1) + error_diffused * 3.;
  img.at<uchar> (i + 2, j + 2) = img.at<uchar> (i + 2, j + 2) + error_diffused * 1.;
};

cv::Mat
dither (cv::Mat& src, const kernel_t kernel) {
  cv::Mat dithered = src.clone();

  for (int i = 0; i < dithered.rows; ++i) {
    for (int j = 0; j < dithered.cols; ++j) {
      auto diff = update_and_diff_closest_palette_color (dithered, i, j);
      kernel (dithered, i, j, diff);
    }
  }

  return dithered;
}

int
main (int argc, char* argv[]) {
  if (argc != 2) {
    std::cout << "usage: DisplayImage.out <Image_Path>\n";
    return -1;
  }
  cv::Mat image;
  image = cv::imread (argv[1], 1);
  if (!image.data) {
    std::cout << "No image data \n";
    return -1;
  }

  cv::Mat gray;
  cv::cvtColor (image, gray, cv::COLOR_BGR2GRAY);
  namedWindow ("Grayscale Image", cv::WINDOW_AUTOSIZE);
  imshow ("Grayscale Image", gray);

  cv::Mat floyd_steinberg = dither (gray, kernel_floyd_steinberg);
  namedWindow ("Floyd-Steinberg Dithered Image", cv::WINDOW_AUTOSIZE);
  imshow ("Floyd-Steinberg Dithered Image", floyd_steinberg);
  cv::imwrite ("floyd-steinberg.jpg", floyd_steinberg);

  cv::Mat atkinson = dither (gray, kernel_atkinson);
  namedWindow ("Atkinson Dithered Image", cv::WINDOW_AUTOSIZE);
  imshow ("Atkinson Dithered Image", atkinson);
  cv::imwrite ("atkinson.jpg", atkinson);

  cv::Mat min_avg_err = dither (gray, kernel_min_average_error);
  namedWindow ("Minimum Average Error Dithered Image", cv::WINDOW_AUTOSIZE);
  imshow ("Minimum Average Error Dithered Image", min_avg_err);
  cv::imwrite ("min_avg_err.jpg", min_avg_err);

  cv::waitKey (0);

  return 0;
}
