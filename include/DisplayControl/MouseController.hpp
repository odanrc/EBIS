/*
 Class: Mouse Controller

 Author: Daniel R. Carvalho

 Last Updated: 05/27/2015

 Function: Handles mouse cursor movement using X11.
*/

#ifndef MOUSE_CONTROLLER_HPP
#define MOUSE_CONTROLLER_HPP

#include <iostream>
#include <cstdlib>
#include <opencv/highgui.h>

#include <X11/Xos.h>
#include <X11/Xutil.h>
#include <X11/Xlib.h>

class MouseController{
  public:
    MouseController();
    ~MouseController();

    void moveCursor(int x, int y);
    void warpCursor(int x, int y);
    void click();
    cv::Point getPos() {return cursor;}

  private:
    Display *dsp;
    Window rootwin;
    XWindowAttributes attr;

    cv::Point cursor;

    // Copy constructor is disabled
    MouseController(const MouseController& copy);
};

#endif
