#include <DisplayControl/ZoomGrid.hpp>

ZoomGrid::ZoomGrid(unsigned int rows, unsigned int cols, unsigned int width, unsigned int height){
  if (rows   == 0) rows   = 1;
  if (cols   == 0) cols   = 1;
  if (width  == 0) width  = 1;
  if (height == 0) height = 1;

  numRows      = rows;
  numCols      = cols;
  this->width  = width;
  this->height = height;

  // Create zoom grid
  for (int i = 0; i < numRows; i++){
    std::vector<ZoomGridRect> line;
    for (int j = 0; j < numCols; j++){
      ZoomGridRect rect;
      rect.x = j*(width/numRows);
      rect.y = i*(height/numCols);
      rect.width = (j+1)*(width/numRows) - rect.x;
      rect.height = (i+1)*(height/numCols) - rect.y;

      line.push_back(rect);
    }
    grid.push_back(line);
  }

  reset();
}

void ZoomGrid::getZoomRect(int &x, int &y, int &width, int &height){
  x = zoomRect.x;
  y = zoomRect.y;
  width = zoomRect.width;
  height = zoomRect.height;
}

bool ZoomGrid::isWithin(ZoomGridRect rect, int x, int y){
  return (x >= rect.x)&&(x <= rect.x + rect.width)&&(y >= rect.y)&&(y <= rect.y + rect.height);
}

void ZoomGrid::zoom(int x, int y){
  for (int i = 0; i < numRows; i++)
    for (int j = 0; j < numCols; j++)
      if (isWithin(grid[i][j], x, y)){
        zoomRect.x     += j*(zoomRect.width/numRows);
        zoomRect.y     += i*(zoomRect.height/numCols);
        zoomRect.width  = zoomRect.width/numRows + zoomRect.width%numRows;
        zoomRect.height = zoomRect.height/numCols + zoomRect.height%numCols;

        if (zoomRect.width == 0) zoomRect.width = 1;
        if (zoomRect.height == 0) zoomRect.height = 1;

        return;
      }
}

void ZoomGrid::reset(){
  zoomRect.x      = 0;
  zoomRect.y      = 0;
  zoomRect.width  = width;
  zoomRect.height = height;
}
