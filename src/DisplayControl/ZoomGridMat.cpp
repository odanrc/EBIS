#include <DisplayControl/ZoomGrid.hpp>
#include <iostream>

ZoomGrid::ZoomGrid(unsigned int rows, unsigned int cols, unsigned int width, unsigned int height){
  if (rows == 0) rows = 1;
  if (cols == 0) cols = 1;
  numRows = rows;
  numCols = cols;
  this->width = width;
  this->height = height;

  // Create zoom grid
  for (int i = 0; i < numRows; i++){
    std::vector<cv::Rect>line;
    for (int j = 0; j < numCols; j++)
      line.push_back(cv::Rect(cv::Point(j*(width/numRows), i*(height/numCols)), cv::Point((j+1)*(width/numRows), (i+1)*(height/numCols))));
    grid.push_back(line);
  }

  reset();
}

cv::Mat ZoomGrid::getZoomImage(cv::Mat image, cv::Point p){
  cv::Mat zoomImage = image(zoomRect);

  // If mat isn't big enough, resize it
  if ((zoomImage.cols < width)||(zoomImage.rows < height))
    cv::resize(zoomImage, zoomImage, cv::Size(width, height));

  for (int i = 0; i < numRows; i++)
    for (int j = 0; j < numCols; j++){
      if (isWithin(grid[i][j], p)){
        double alpha = 0.1;
        cv::Mat roi = zoomImage(grid[i][j]);
        cv::Mat color(roi.size(), CV_8UC4, CV_RGB(0, 0, 150)); 
        cv::addWeighted(color, alpha, roi, 1.0 - alpha , 0.0, roi); 
      } else {
      cv::rectangle(zoomImage, grid[i][j], CV_RGB(0, 0, 0));
      }
    }

  return zoomImage;
}

bool ZoomGrid::isWithin(cv::Rect rect, cv::Point p){
  return (p.x >= rect.x)&&(p.x <= rect.x + rect.width)&&(p.y >= rect.y)&&(p.y <= rect.y + rect.height);
}

void ZoomGrid::zoom(cv::Point p){
  for (int i = 0; i < numRows; i++)
    for (int j = 0; j < numCols; j++)
      if (isWithin(grid[i][j], p)){
        zoomRect = cv::Rect(zoomRect.x + j*(zoomRect.width/numRows),
                  zoomRect.y + i*(zoomRect.height/numCols),
                  zoomRect.width/numRows + zoomRect.width%numRows,
                  zoomRect.height/numCols + zoomRect.height%numCols);

        return;
      }
}

void ZoomGrid::reset(){
  zoomRect = cv::Rect(0, 0, width, height);
}
