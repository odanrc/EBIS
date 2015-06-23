#include "DisplayControl/SDLController.hpp"

#include <iostream>

SDLController::SDLController(int numRows, int numCols){
  numGridRows = numRows;
  numGridCols = numCols;

  // Show grid by default
  displayGrid = true;

  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) != 0) throw "Error initializing SDL";

  int screenWidth, screenHeight;
  getWindowSize(screenWidth, screenHeight);

  // Create a fullscreen window
  sdlWindow = SDL_CreateWindow(
      "Meye", 0, 0, screenWidth, screenHeight,
      SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_BORDERLESS | SDL_WINDOW_FULLSCREEN_DESKTOP
  );

  // Check if window was successfully created
  if (sdlWindow == NULL) throw "Could not initialize display control.";

  // Create renderer
  sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, 0);
  // Check if renderer was successfully created
  if (sdlRenderer == NULL) throw "Could not initialize display control.";

  sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, screenWidth, screenHeight);
  if (sdlTexture == NULL) throw "Could not initialize display control.";

  // Set render clear color
  SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);  

  // Put mouse in the middle of the screen
  warpMouse(screenWidth/2, screenHeight/2);

  hide();
}

SDLController::~SDLController(){
  if (sdlTexture)  SDL_DestroyTexture(sdlTexture);
  if (sdlRenderer) SDL_DestroyRenderer(sdlRenderer);
  if (sdlWindow)   SDL_DestroyWindow(sdlWindow);
  SDL_Quit();
}

void SDLController::showGrid(){
  displayGrid = true;
}

void SDLController::hideGrid(){
  displayGrid = false;
}

void SDLController::render(SDL_Surface *sdlSurface, SDL_Rect *zoomRect){
  if (sdlSurface){
    sdlSurface = getZoomImage(sdlSurface, zoomRect);

    // Transform surface into texture
    SDL_UpdateTexture(sdlTexture, NULL, sdlSurface->pixels, sdlSurface->pitch);

    // Clear screen
    SDL_RenderClear(sdlRenderer);

    // Copy display data to renderer
    SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);

    // Display grid
    if (displayGrid){
      int screenWidth, screenHeight;
      getWindowSize(screenWidth, screenHeight);

      for (int x = 1; x < numGridCols; x++)
        SDL_RenderDrawLine(sdlRenderer, x*screenWidth/numGridCols, 0, x*screenWidth/numGridCols, screenHeight);

      for (int y = 1; y < numGridRows; y++)
        SDL_RenderDrawLine(sdlRenderer, 0, y*screenHeight/numGridRows, screenWidth, y*screenHeight/numGridRows);
    }

    // Render
    SDL_RenderPresent(sdlRenderer);

    // Free surface
    SDL_FreeSurface(sdlSurface);
  } else std::cout << SDL_GetError() << std::endl;
}

SDL_Surface* SDLController::getZoomImage(SDL_Surface *image, SDL_Rect *zoomRect){
  int screenWidth, screenHeight;
  getWindowSize(screenWidth, screenHeight);

  SDL_Surface *zoomImage = SDL_CreateRGBSurface(image->flags, screenWidth, screenHeight, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0);

  // If image isn't big enough to cover screen, resize it
  SDL_BlitScaled(image, zoomRect, zoomImage, NULL);

  return zoomImage;
}

void SDLController::hide(){
  SDL_HideWindow(sdlWindow);
}

void SDLController::show(){
  SDL_ShowWindow(sdlWindow);
}

void SDLController::getWindowSize(int &width, int &height){
  SDL_DisplayMode displayMode;
  SDL_GetDesktopDisplayMode(0, &displayMode);

  width = displayMode.w;
  height = displayMode.h;
}

void SDLController::showMessage(const char *message){
  SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Excuse me, but...", message, NULL);
}

void SDLController::warpMouse(int x, int y){
  // Check for invalid input
  int width, height;
  getWindowSize(width, height);

  if (x < 0) x = 0;
  else if (x >= width) x = width-1;

  if (y < 0) y = 0;
  else if (y >= height) y = height-1;

  mouse.x = x;
  mouse.y = y;

  SDL_WarpMouseInWindow(sdlWindow, x, y);
}

void SDLController::moveMouse(int diffX, int diffY){
  warpMouse(mouse.x + diffX, mouse.y + diffY);
}

void SDLController::getMousePos(int &x, int &y){
  x = mouse.x;
  y = mouse.y;
}
