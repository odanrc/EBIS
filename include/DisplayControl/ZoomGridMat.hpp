/*
 Class: Zoom Grid

 Author: Daniel R. Carvalho

 Last Updated: 05/27/2015

 Function: Handles the zoom grid and its magnifications using OpenCV Mats.
*/

#ifndef ZOOM_GRID_HPP
#define ZOOM_GRID_HPP

#include <vector>

#include "opencv/highgui.h"
#include "opencv2/imgproc/imgproc.hpp"

class ZoomGrid{
private:
  int numRows, numCols, width, height;
  std::vector<std::vector<cv::Rect> > zoomGrid;
  std::vector<std::vector<cv::Rect> > grid;
  cv::Rect zoomRect;

  ZoomGrid(){}
  bool isWithin(cv::Rect rect, cv::Point p);

public:  
  ZoomGrid(unsigned int rows, unsigned int cols, unsigned int width, unsigned int height);
  cv::Mat getZoomImage(cv::Mat image, cv::Point p);
  void zoom(cv::Point p);
  void reset();
};

#endif