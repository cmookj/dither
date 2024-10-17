#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>

enum class dither_t { floyd_steinberg, atkinson, min_average_error };

cv::Mat
dither (cv::Mat& src, const dither_t method) {
  cv::Mat dithered = src.clone();

  switch (method) {
  case dither_t::floyd_steinberg:
    for (int i = 0; i < dithered.rows; ++i) {
      for (int j = 0; j < dithered.cols; ++j) {
        auto old_pixel = dithered.at<uchar> (i, j);
        auto new_pixel = (old_pixel > 128 ? 255 : 0);

        dithered.at<uchar> (i, j) = new_pixel;

        auto error_diffused = (old_pixel - new_pixel) / 16.;

        dithered.at<uchar> (i, j + 1)     = dithered.at<uchar> (i, j + 1) + error_diffused * 7.;
        dithered.at<uchar> (i + 1, j - 1) = dithered.at<uchar> (i + 1, j - 1) + error_diffused * 3.;
        dithered.at<uchar> (i + 1, j)     = dithered.at<uchar> (i + 1, j) + error_diffused * 5.;
        dithered.at<uchar> (i + 1, j + 1) = dithered.at<uchar> (i + 1, j + 1) + error_diffused;
      }
    }

    break;

  case dither_t::atkinson:
    for (int i = 0; i < dithered.rows; ++i) {
      for (int j = 0; j < dithered.cols; ++j) {
        auto old_pixel = dithered.at<uchar> (i, j);
        auto new_pixel = (old_pixel > 128 ? 255 : 0);

        dithered.at<uchar> (i, j) = new_pixel;

        auto error_diffused = (old_pixel - new_pixel) / 8.;

        dithered.at<uchar> (i, j + 1) = dithered.at<uchar> (i, j + 1) + error_diffused;
        dithered.at<uchar> (i, j + 2) = dithered.at<uchar> (i, j + 2) + error_diffused;

        dithered.at<uchar> (i + 1, j - 1) = dithered.at<uchar> (i + 1, j - 1) + error_diffused;
        dithered.at<uchar> (i + 1, j)     = dithered.at<uchar> (i + 1, j) + error_diffused;
        dithered.at<uchar> (i + 1, j + 1) = dithered.at<uchar> (i + 1, j + 1) + error_diffused;

        dithered.at<uchar> (i + 2, j) = dithered.at<uchar> (i + 2, j) + error_diffused;
      }
    }

    break;

  case dither_t::min_average_error:
    for (int i = 0; i < dithered.rows; ++i) {
      for (int j = 0; j < dithered.cols; ++j) {
        auto old_pixel = dithered.at<uchar> (i, j);
        auto new_pixel = (old_pixel > 128 ? 255 : 0);

        dithered.at<uchar> (i, j) = new_pixel;

        auto error_diffused = (old_pixel - new_pixel) / 48.;

        dithered.at<uchar> (i, j + 1) = dithered.at<uchar> (i, j + 1) + error_diffused * 7.;
        dithered.at<uchar> (i, j + 2) = dithered.at<uchar> (i, j + 2) + error_diffused * 5.;

        dithered.at<uchar> (i + 1, j - 2) = dithered.at<uchar> (i + 1, j - 2) + error_diffused * 3.;
        dithered.at<uchar> (i + 1, j - 1) = dithered.at<uchar> (i + 1, j - 1) + error_diffused * 5.;
        dithered.at<uchar> (i + 1, j)     = dithered.at<uchar> (i + 1, j) + error_diffused * 7.;
        dithered.at<uchar> (i + 1, j + 1) = dithered.at<uchar> (i + 1, j + 1) + error_diffused * 5.;
        dithered.at<uchar> (i + 1, j + 2) = dithered.at<uchar> (i + 1, j + 2) + error_diffused * 3.;

        dithered.at<uchar> (i + 2, j - 2) = dithered.at<uchar> (i + 2, j - 2) + error_diffused * 1.;
        dithered.at<uchar> (i + 2, j - 1) = dithered.at<uchar> (i + 2, j - 1) + error_diffused * 3.;
        dithered.at<uchar> (i + 2, j)     = dithered.at<uchar> (i + 2, j) + error_diffused * 5.;
        dithered.at<uchar> (i + 2, j + 1) = dithered.at<uchar> (i + 2, j + 1) + error_diffused * 3.;
        dithered.at<uchar> (i + 2, j + 2) = dithered.at<uchar> (i + 2, j + 2) + error_diffused * 1.;
      }
    }

    break;
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

  cv::Mat floyd_steinberg = dither (gray, dither_t::floyd_steinberg);
  namedWindow ("Floyd-Steinberg Dithered Image", cv::WINDOW_AUTOSIZE);
  imshow ("Floyd-Steinberg Dithered Image", floyd_steinberg);
  cv::imwrite ("floyd-steinberg.jpg", floyd_steinberg);

  cv::Mat atkinson = dither (gray, dither_t::atkinson);
  namedWindow ("Atkinson Dithered Image", cv::WINDOW_AUTOSIZE);
  imshow ("Atkinson Dithered Image", atkinson);
  cv::imwrite ("atkinson.jpg", atkinson);

  cv::Mat min_avg_err = dither (gray, dither_t::min_average_error);
  namedWindow ("Minimum Average Error Dithered Image", cv::WINDOW_AUTOSIZE);
  imshow ("Minimum Average Error Dithered Image", min_avg_err);
  cv::imwrite ("min_avg_err.jpg", min_avg_err);

  cv::waitKey (0);

  return 0;
}
