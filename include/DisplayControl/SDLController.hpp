/*
 Class: SDL Controller

 Author: Daniel R. Carvalho

 Last Updated: 05/27/2015

 Function: Uses SDL 2 to create a grid window and handle mouse positioning.
*/

#ifndef SDL_CONTROLLER_HPP
#define SDL_CONTROLLER_HPP

#include "SDL2/SDL.h"
#include "ZoomGrid.hpp"

class SDLController{
private:
  SDL_Window *sdlWindow;
  SDL_Texture *sdlTexture;
  SDL_Renderer *sdlRenderer;

  SDL_Point mouse;

  // Grid size
  int numGridRows, numGridCols;

  bool displayGrid;

  SDLController(){}

public:
  SDLController(int numRows, int numCols);
  ~SDLController();

  // Render image to window
  void render(SDL_Surface *sdlSurface, SDL_Rect *zoomRect);
  // Get image after applying zoom rectangle. The image after the zoom has the same size of the original image
  SDL_Surface* getZoomImage(SDL_Surface *image, SDL_Rect *zoomRect);

  // Show message box
  void showMessage(const char *message);

  // Get windows size (desktop resolution)
  void getWindowSize(int &width, int &height);
  // Hide window
  void hide();
  // Show window
  void show();

  // Warp mouse to point
  void warpMouse(int x, int y);
  // Move mouse an offset from current position
  void moveMouse(int diffX, int diffY);
  // Get mouse position
  void getMousePos(int &x, int &y);

  // Show/hide grid lines
  void showGrid();
  void hideGrid();

};

#endif