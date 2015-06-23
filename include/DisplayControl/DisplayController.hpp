/*
 Class: Display Controller

 Author: Daniel R. Carvalho

 Last Updated: 05/27/2015

 Function: Controls zoom grid display and mouse movement.
*/

#ifndef DISPLAY_CONTROL_HPP
#define DISPLAY_CONTROL_HPP

#include "SDL2/SDL.h"

#include "opencv/highgui.h"

#include "SDLController.hpp"
#include "ZoomGrid.hpp"

#ifdef __linux__
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

#ifdef WIN32
#include <windows.h>
#endif

class DisplayController{
private:
    SDLController *sdlController;

    ZoomGrid *zoomGrid;

    SDL_Surface *desktopImage;

    // Control flag for grid display
    bool displayGrid;

    #ifdef __linux__
    Display *xDisplay;
    #endif

    // This is used by the progressive zoom functions
    SDL_Rect zoomArea;

public:
  // These are compliant with X11
  typedef enum {LEFT_CLICK = 1, MIDDLE_CLICK, RIGHT_CLICK, SCROLL_UP, SCROLL_DOWN} MouseButton;

  DisplayController(int numRows = 3, int numCols = 3);
  ~DisplayController();

  void display();
  // Take a screenshot of desktop and display it, so that user can choose where to zoom/click
  void start();

  // Zoom at the current mouse position
  void zoom();

  // Stop and hide window
  void stop();

  // Get screenshot and save it in internal variable
  void takeScreenshot();
  // Get window size
  void getWindowSize(int &width, int &height);

  // Show message box
  void showMessage(const char *message);

  // Warp mouse to point
  void warpMouse(int x, int y);
  // Move mouse an offset from current position
  void moveMouse(int diffX, int diffY);
  // Get mouse position
  void getMousePos(int &x, int &y);

  // Click mouse in the middle of the current zoom rectangle
  void clickMouse(MouseButton button);

  // Show/Hide grid lines
  void showGrid();
  void hideGrid();

  // These two functions handle zoom as a progression of minimal zooms at the observed location
  // These are not used because the low fps may cause it to be too slow to be used
  void startProgressiveZoom();
  bool progressiveZoom();
};

#endif