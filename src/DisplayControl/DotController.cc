#include <DisplayControl/DotController.hpp>

DotController::DotController(){
  init(640, 480);
}

DotController::DotController(int width, int height){
  init(width, height);
}

void DotController::init(int width, int height){
  srand(time(NULL));
  this->width = width;
  this->height = height;
  generate();
}

void DotController::generate(){
  point = cv::Point(rand()%width, rand()%height);
}

bool DotController::check(cv::Point p){
  if ((p.x < ((point.x > 0) && (DOT_ERROR_MARGIN > INT_MAX - point.x)) ? 0 : point.x + DOT_ERROR_MARGIN)&&
      (p.x > (point.x >= DOT_ERROR_MARGIN)                             ? point.x - DOT_ERROR_MARGIN : 0)&&
      (p.y < ((point.y > 0) && (DOT_ERROR_MARGIN > INT_MAX - point.y)) ? 0 : point.y + DOT_ERROR_MARGIN)&&
      (p.y > (point.y >= DOT_ERROR_MARGIN)                             ? point.y - DOT_ERROR_MARGIN : 0)){
    generate();
    return true;
  }

  return false;
}

cv::Point DotController::getPoint(){
  return point;
}