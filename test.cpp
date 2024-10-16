#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>

cv::Mat
floyd_steinberg_dither (cv::Mat& src) {
  cv::Mat dithered = src.clone();

  for (int i = 0; i < dithered.rows; ++i) {
    for (int j = 0; j < dithered.cols; ++j) {
      auto old_pixel = dithered.at<uchar> (i, j);
      auto new_pixel = (old_pixel > 128 ? 255 : 0);

      dithered.at<uchar> (i, j) = new_pixel;

      auto quantization_error = old_pixel - new_pixel;

      dithered.at<uchar> (i, j + 1) = dithered.at<uchar> (i, j + 1) + quantization_error * 7. / 16.;
      dithered.at<uchar> (i + 1, j - 1) =
          dithered.at<uchar> (i + 1, j - 1) + quantization_error * 3. / 16.;
      dithered.at<uchar> (i + 1, j) = dithered.at<uchar> (i + 1, j) + quantization_error * 5. / 16.;
      dithered.at<uchar> (i + 1, j + 1) =
          dithered.at<uchar> (i + 1, j + 1) + quantization_error * 1. / 16.;
    }
  }

  return dithered;
}

cv::Mat
atkinson_dither (cv::Mat& src) {
  cv::Mat dithered = src.clone();

  for (int i = 0; i < dithered.rows; ++i) {
    for (int j = 0; j < dithered.cols; ++j) {
      auto old_pixel = dithered.at<uchar> (i, j);
      auto new_pixel = (old_pixel > 128 ? 255 : 0);

      dithered.at<uchar> (i, j) = new_pixel;

      auto error_diffused = 1. / 8. * (old_pixel - new_pixel);

      dithered.at<uchar> (i, j + 1) = dithered.at<uchar> (i, j + 1) + error_diffused;
      dithered.at<uchar> (i, j + 2) = dithered.at<uchar> (i, j + 2) + error_diffused;

      dithered.at<uchar> (i + 1, j - 1) = dithered.at<uchar> (i + 1, j - 1) + error_diffused;
      dithered.at<uchar> (i + 1, j)     = dithered.at<uchar> (i + 1, j) + error_diffused;
      dithered.at<uchar> (i + 1, j + 1) = dithered.at<uchar> (i + 1, j + 1) + error_diffused;

      dithered.at<uchar> (i + 2, j) = dithered.at<uchar> (i + 2, j) + error_diffused;
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

  cv::Mat floyd_steinberg = floyd_steinberg_dither (gray);
  namedWindow ("Floyd-Steinberg Dithered Image", cv::WINDOW_AUTOSIZE);
  imshow ("Floyd-Steinberg Dithered Image", floyd_steinberg);
  cv::imwrite ("floyd-steinberg.jpg", floyd_steinberg);

  cv::Mat atkinson = atkinson_dither (gray);
  namedWindow ("Atkinson Dithered Image", cv::WINDOW_AUTOSIZE);
  imshow ("Atkinson Dithered Image", atkinson);
  cv::imwrite ("atkinson.jpg", atkinson);

  cv::waitKey (0);

  return 0;
}
