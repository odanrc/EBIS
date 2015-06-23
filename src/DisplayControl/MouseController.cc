#include <DisplayControl/MouseController.hpp>

MouseController::MouseController(){
  if (!(dsp = XOpenDisplay(""))){
    std::cout << "Could not connect to X11 display!\n";
    exit (EXIT_FAILURE);
  }
  rootwin = DefaultRootWindow(dsp);
  XGetWindowAttributes(dsp, rootwin, &(attr));

  // Set initial mouse position as center of screen
  cursor.x = (int)attr.width/2;
  cursor.y = (int)attr.height/2;

  // Move mouse to center of screen
  moveCursor(0, 0);
}

void MouseController::moveCursor(int diffX, int diffY) {
  cursor.x += diffX;
  cursor.y += diffY;

  if (cursor.x < 0) cursor.x = 0;
  if (cursor.x >= attr.width) cursor.x = attr.width-1;
  if (cursor.y < 0) cursor.y = 0;
  if (cursor.y >= attr.height) cursor.y = attr.height-1;

  XWarpPointer(dsp, None, rootwin, 0, 0, 0, 0, cursor.x, cursor.y);
  XFlush(dsp);
}

void MouseController::warpCursor(int x, int y){
  cursor.x = attr.width/2 + x;
  cursor.y = attr.height/2 + y;

  if (cursor.x < 0) cursor.x = 0;
  if (cursor.x >= attr.width) cursor.x = attr.width-1;
  if (cursor.y < 0) cursor.y = 0;
  if (cursor.y >= attr.height) cursor.y = attr.height-1;

  XWarpPointer(dsp, None, rootwin, 0, 0, 0, 0, cursor.x, cursor.y);
  XFlush(dsp);  
}

void MouseController::click() {
/*
  static int press = 0;
  if (press) {
    XTestFakeButtonEvent (dsp, 1, False, 10 );
    press=1;
  }
  else {
    XTestFakeButtonEvent (dsp, 1, True, 10 );
    press=0;
  }
*/
  XFlush(dsp);
}

MouseController::~MouseController(){
  XCloseDisplay(dsp);
}

/*
int main(){
  MouseController mc;
  
  mc.click();
  
  return 0;
}*/
