/*
 Class: Zoom Grid

 Author: Daniel R. Carvalho

 Last Updated: 05/27/2015

 Function: Handles the zoom grid and its magnifications.
*/

#ifndef ZOOM_GRID_HPP
#define ZOOM_GRID_HPP

#include <vector>

class ZoomGrid{
private:
  int numRows, numCols, width, height;
  typedef struct {
    int x;
    int y;
    int width;
    int height;
  } ZoomGridRect;
  ZoomGridRect zoomRect;

  // The grid
  std::vector<std::vector<ZoomGridRect> > grid;

  ZoomGrid(){}

  // Checks if a point is inside a rectangle.
  bool isWithin(ZoomGridRect rect, int x, int y);

public:  
  // Initialize a rows x cols grid to cover a screen of width x height.
  ZoomGrid(unsigned int rows, unsigned int cols, unsigned int width, unsigned int height);

  // Get rectangle representing the zoomed area.
  void getZoomRect(int &x, int &y, int &width, int &height);

  // Zoom into the rectangle containing the given point.
  void zoom(int x, int y);

  // Reset zoom. Zoom rectangle becomes the whole screen.
  void reset();
};

#endif