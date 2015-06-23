#include <DisplayControl/DisplayController.hpp>

#include <iostream>

static const char *CLICK_ERROR_MESSAGE = "Failed to send click event";
static const char *DISPLAY_ERROR_MESSAGE = "Failed to initialize display controller";

DisplayController::DisplayController(int numRows, int numCols){
  #ifdef __linux__
    // Initialize X11
    xDisplay = XOpenDisplay(NULL);
    if(xDisplay == NULL){
      showMessage(DISPLAY_ERROR_MESSAGE);
      exit(EXIT_FAILURE);
    }
  #endif

  #ifdef WIN32
    // TODO
  #endif  

  sdlController = new SDLController(numRows, numCols);

  int screenWidth, screenHeight;
  sdlController->getWindowSize(screenWidth, screenHeight);

  // Initialize zoom grid
  zoomGrid = new ZoomGrid(numRows, numCols, screenWidth, screenHeight);

  desktopImage = NULL;

  zoomArea.x = 0;
  zoomArea.y = 0;
  zoomArea.w = screenWidth;
  zoomArea.h = screenHeight;
}

DisplayController::~DisplayController(){
  #ifdef __linux__
    XCloseDisplay(xDisplay);
  #endif
}

void DisplayController::takeScreenshot(){
  int screenWidth, screenHeight;
  sdlController->getWindowSize(screenWidth, screenHeight);

  #ifdef __linux__
    // Get screenshot
    XImage *image = XGetImage(xDisplay, RootWindow(xDisplay, DefaultScreen(xDisplay)), 0, 0, screenWidth, screenHeight, AllPlanes, ZPixmap);

    // If there was a image stored, free it
    if (desktopImage) SDL_FreeSurface(desktopImage);

    // Create empty image
    desktopImage = SDL_CreateRGBSurface(0, screenWidth, screenHeight, image->bits_per_pixel, image->red_mask, image->green_mask, image->blue_mask, 0);

    // Copy screenshot data to sdl surface
    SDL_LockSurface(desktopImage);
    for (int y = 0; y < screenHeight; y++) {
        for (int x = 0; x < screenWidth; x++) {
            ((unsigned *)desktopImage->pixels)[x + y * screenWidth] = XGetPixel(image, x, y);
        }
    }
    SDL_UnlockSurface(desktopImage); 
  #endif
}

void DisplayController::getWindowSize(int &width, int &height){
  sdlController->getWindowSize(width, height);
}

void DisplayController::showMessage(const char *message){
  sdlController->showMessage(message);
}

void DisplayController::showGrid(){
  sdlController->showGrid();
}

void DisplayController::hideGrid(){
  sdlController->hideGrid();
}

void DisplayController::zoom(){
  int x, y;
  getMousePos(x, y);
  zoomGrid->zoom(x, y);

  SDL_Rect r;
  zoomGrid->getZoomRect(r.x, r.y, r.w, r.h);
  sdlController->render(desktopImage, &r);
}

void DisplayController::start(){
  zoomGrid->reset();

  stop();
  SDL_Delay(100);
  takeScreenshot();

  SDL_Rect r;
  zoomGrid->getZoomRect(r.x, r.y, r.w, r.h);
  sdlController->show();
  sdlController->render(desktopImage, &r);
}

void DisplayController::stop(){
  sdlController->hide();
}

void DisplayController::warpMouse(int x, int y){
  sdlController->warpMouse(x, y);
}

void DisplayController::moveMouse(int diffX, int diffY){
  sdlController->moveMouse(diffX, diffY);
}

void DisplayController::getMousePos(int &x, int &y){
  sdlController->getMousePos(x, y);
}

// TODO set scroll amount
// TODO fix this. It is not clicking inside windows because most windows ignore XSendEvent
void DisplayController::clickMouse(MouseButton button){
  int x, y, w, h;
  zoomGrid->getZoomRect(x, y, w, h);

  // Point to be clicked is at center of zoom rectangle
  if (!((button == SCROLL_UP)||(button == SCROLL_DOWN))) warpMouse(x + w/2, y + h/2);

  // Stop zoomming
  stop();
  SDL_Delay(10);

  #ifdef __linux__
    XEvent event;

    memset(&event, 0x00, sizeof(event));
    
    event.type = ButtonPress;
    event.xbutton.button = button;
    event.xbutton.same_screen = True;
    
    XQueryPointer(xDisplay, RootWindow(xDisplay, DefaultScreen(xDisplay)), &event.xbutton.root, &event.xbutton.window, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);

    event.xbutton.subwindow = event.xbutton.window;
    
    while(event.xbutton.subwindow){
      event.xbutton.window = event.xbutton.subwindow;
      
      XQueryPointer(xDisplay, event.xbutton.window, &event.xbutton.root, &event.xbutton.subwindow, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
    }

    if(XSendEvent(xDisplay, PointerWindow, True, 0xfff, &event) == 0) showMessage(CLICK_ERROR_MESSAGE);
    
    XFlush(xDisplay);

    event.type = ButtonRelease;
    event.xbutton.state = 0x100;
    
    if(XSendEvent(xDisplay, PointerWindow, True, 0xfff, &event) == 0) showMessage(CLICK_ERROR_MESSAGE);
    
    XFlush(xDisplay);    
  #endif

  #ifdef WIN32
    // TODO test

    INPUT input={0};
    input.type = INPUT_MOUSE;

    const DWORD SCROLL_MOVEMENT = 30; 

    switch (button){
      case LEFT_CLICK:
        input.mi.dwFlags   = MOUSEEVENTF_LEFTDOWN;
        break;
      case MIDDLE_CLICK:
        input.mi.dwFlags   = MOUSEEVENTF_MIDDLEDOWN;
        break;
      case RIGHT_CLICK:
        input.mi.dwFlags   = MOUSEEVENTF_RIGHTDOWN;
        break;
      case SCROLL_UP:
        input.mi.dwFlags   = MOUSEEVENTF_WHEEL;
        input.mi.mouseData = SCROLL_MOVEMENT;
        break;
      case SCROLL_DOWN:
        Input.mi.dwFlags   = MOUSEEVENTF_WHEEL;
        input.mi.mouseData = -SCROLL_MOVEMENT;
        break;
    }

    SendInput(1, &input, sizeof(INPUT));

    ZeroMemory(&input,sizeof(INPUT));
    input.type        = INPUT_MOUSE;

    // If event was a mouse click, lift the button 
    switch (button){
      case LEFT_CLICK:
        input.mi.dwFlags  = MOUSEEVENTF_LEFTUP;
        SendInput(1, &input, sizeof(iNPUT));
        break;
      case MIDDLE_CLICK:
        input.mi.dwFlags  = MOUSEEVENTF_MIDDLEUP;
        SendInput(1, &input, sizeof(iNPUT));
        break;
      case RIGHT_CLICK:
        input.mi.dwFlags  = MOUSEEVENTF_RIGHTUP;
        SendInput(1, &input, sizeof(iNPUT));
        break;
    }
  #endif
}

void DisplayController::startProgressiveZoom(){
  stop();
  SDL_Delay(100);
  takeScreenshot();

  int screenWidth, screenHeight;
  sdlController->getWindowSize(screenWidth, screenHeight);

  zoomArea.x = 0;
  zoomArea.y = 0;
  zoomArea.w = screenWidth;
  zoomArea.h = screenHeight;

  sdlController->render(desktopImage, &zoomArea);
  sdlController->show();
}

bool DisplayController::progressiveZoom(){
  int x, y, mappedX, mappedY;
  int screenWidth, screenHeight;

  getMousePos(x, y);
  sdlController->getWindowSize(screenWidth, screenHeight);

  mappedX = x*zoomArea.w/screenWidth;
  mappedY = y*zoomArea.h/screenHeight;

  int ZOOM_PRECISION = 10;
  zoomArea.w -= ZOOM_PRECISION;
  zoomArea.h -= ZOOM_PRECISION;
  if (x > screenWidth/2) zoomArea.x += ZOOM_PRECISION;
  if (y > screenHeight/2) zoomArea.y += ZOOM_PRECISION;

  if (zoomArea.x < 0) zoomArea.x = 0;
  if (zoomArea.y < 0) zoomArea.y = 0;
  if (zoomArea.x + zoomArea.w > screenWidth) zoomArea.x = screenWidth - zoomArea.w;
  if (zoomArea.y + zoomArea.h > screenWidth) zoomArea.y = screenWidth - zoomArea.h;
  std::cout << zoomArea.x << " " << zoomArea.y << " " << zoomArea.w << " " << zoomArea.h << std::endl;
  if ((zoomArea.w <= ZOOM_PRECISION)||(zoomArea.h <= ZOOM_PRECISION)) {
    stop();
    return true;
  }

  sdlController->render(desktopImage, &zoomArea);

  return false;
}
