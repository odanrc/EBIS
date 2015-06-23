/*
 Class: Dot Controller

 Author: Daniel R. Carvalho

 Last Updated: 05/27/2015

 Function: Handles and creates dots at random positions.
*/

#ifndef DOT_CONTROLLER_H
#define DOT_CONTROLLER_H

#include <cstdlib>
#include <ctime>

#include "opencv/highgui.h"

const int DOT_ERROR_MARGIN = 10;

class DotController{
private:
  // The dot
  cv::Point point;

  // Limits
  int width, height;

  // Generate a new dot
  void generate();

  // Initializer
  void init(int width, int height);

public:
  // Initializes the class with width = 640 and height = 480
  DotController();

  // Initializes the class with given size
  DotController(int width, int height);

  // Checks if point p is near dot according to DOT_ERROR_MARGIN
  // If so, a new point is generated
  bool check(cv::Point p);

  // Returns the dot position
  cv::Point getPoint();
};

#endif