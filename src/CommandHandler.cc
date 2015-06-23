#include <CommandHandler.hpp>

#include <iostream>

static const int NUM_GRID_ROWS = 3;
static const int NUM_GRID_COLS = 3;
static const double BLINK_PERCENTAGE = 0.3;

// Average mouse precision using regular methods (touchpad, mouse)
static const int MOUSE_PRECISION = 3;

// If the user looks past this offset, the line is changed
static const double CHANGE_LINE_OFFSET = 0.1;

// Position smoothing filter's size
static const int SMOOTHING_FILTER_SIZE = 50;

// Blink smoothing filter's size
static const int BLINK_FILTER_SIZE = 20;

CommandHandler::CommandHandler(){
  programMode = IDLE_MODE;

  modeCmd = false;
  clickCmd = false;
  zoomCounter = 1;

  // Initialize display controller
  displayController = new DisplayController(NUM_GRID_ROWS, NUM_GRID_COLS);

  // Initialize gaze filters
  xFilter = new SmoothingFilter(SMOOTHING_FILTER_SIZE);
  yFilter = new SmoothingFilter(SMOOTHING_FILTER_SIZE);

  // Initialize blink filters
  blinkLFilter = new SmoothingFilter(BLINK_FILTER_SIZE);
  blinkRFilter = new SmoothingFilter(BLINK_FILTER_SIZE);

  // Start timer
  time(&lastCmdTime);
}

CommandHandler::~CommandHandler(){

}

void CommandHandler::switchMode(int screenWidth, int screenHeight){
  if ((!modeCmd)&&(zoomCounter <= 1)){
    modeCmd = true;
    zoomCounter = 1;

    switch (programMode){
      case IDLE_MODE:
        // Start tracker at top
        curGridY = screenHeight/NUM_GRID_ROWS/2;

        displayController->start();
        programMode = LEFT_CLICK_MODE;
        break;
      case LEFT_CLICK_MODE:
        // Start tracker at top
        curGridY = screenHeight/NUM_GRID_ROWS/2;

        displayController->start();
        programMode = SCROLL_MODE;
        break;
      case SCROLL_MODE:
        displayController->stop();
        programMode = IDLE_MODE;
        break;
      // Not implemented yet
      case TYPE_MODE:
        // Start tracker at top
        curGridY = screenHeight/NUM_GRID_ROWS/2;

        displayController->start();
        programMode = RIGHT_CLICK_MODE;
        break;
      // Not implemented yet
      case RIGHT_CLICK_MODE:
        // Start tracker at top
        curGridY = screenHeight/NUM_GRID_ROWS/2;

        displayController->start();
        programMode = DOUBLE_CLICK_MODE;
        break;
      // Not implemented yet
      case DOUBLE_CLICK_MODE:
        displayController->stop();
        programMode = ALT_TAB_MODE;
        break;
      // Not implemented yet
      case ALT_TAB_MODE:
        programMode = IDLE_MODE;
    }
  }
}

void CommandHandler::getWindowSize(int &width, int &height){
  return displayController->getWindowSize(width, height);
}

void CommandHandler::parse(bool leftEyeBlink, bool rightEyeBlink, int focusPointX, int focusPointY){
  time_t curTime; 
  time(&curTime);
  bool cmdAllowed = difftime(curTime, lastCmdTime) > 2.0f;

  int screenWidth, screenHeight;
  getWindowSize(screenWidth, screenHeight);

  // Apply filter to reduce gaze noise
  focusPointX = xFilter->smoothen(focusPointX);
  focusPointY = yFilter->smoothen(focusPointY);

  // Apply filter to avoid false blinks
  leftEyeBlink = blinkLFilter->smoothen(leftEyeBlink);
  rightEyeBlink = blinkRFilter->smoothen(rightEyeBlink);

  // For debugging reasons - TODO remove later
  if (leftEyeBlink) std::cout << "L blink" << std::endl;
  if (rightEyeBlink) std::cout << "R blink" << std::endl;

  // If both eyes are closed, it's a switch mode command
  if (leftEyeBlink && rightEyeBlink && cmdAllowed) {
    switchMode(screenWidth, screenHeight);
    lastCmdTime = curTime;
  // If both eyes are open and a switch mode command has been recently issued
  } else if (!(leftEyeBlink||rightEyeBlink)&&modeCmd){
    // Empty buffers to prevent missinterpretations
    blinkLFilter->clear();
    blinkRFilter->clear();

    // Finishing parsing switch mode command
    modeCmd = false;
  }

  // Parse movement
  if (!modeCmd){
    if (programMode == SCROLL_MODE){
      if (focusPointY > screenHeight*5.0/6.0) focusPointY = screenHeight*5.0/6.0;
      if (focusPointY < screenHeight/6.0) focusPointY = screenHeight/6.0;
      displayController->warpMouse(screenWidth/2, focusPointY);
      // displayController->moveMouse(0, movement.y);

      if (focusPointY >= 5.0*screenHeight/6.0) displayController->clickMouse(displayController->SCROLL_DOWN);
      else if (focusPointY <= screenHeight/6.0) displayController->clickMouse(displayController->SCROLL_UP);
    } else {
      if ((programMode == LEFT_CLICK_MODE)||(programMode == TYPE_MODE)||(programMode == RIGHT_CLICK_MODE)){
        // Update mouse Y
        if (focusPointX > screenWidth * (1-CHANGE_LINE_OFFSET)){
          curGridY += screenHeight/NUM_GRID_ROWS;
          if (curGridY > screenHeight) curGridY -= screenHeight; 

          // Restart filter
          for (int i = 0; i < SMOOTHING_FILTER_SIZE; i++) 
            focusPointX = xFilter->smoothen(screenWidth/NUM_GRID_COLS);
        } else if (focusPointX < screenWidth * CHANGE_LINE_OFFSET){
          curGridY -= screenHeight/NUM_GRID_ROWS;
          if (curGridY < 0) curGridY += screenHeight; 

          for (int i = 0; i < SMOOTHING_FILTER_SIZE; i++) 
            focusPointX = xFilter->smoothen(screenWidth-screenWidth/NUM_GRID_COLS);
        }

        // If one eye is closed, it's a click command
        if (leftEyeBlink||rightEyeBlink){
          // If not already emitting click command, do so
          if (!clickCmd && cmdAllowed) {
            clickCmd = true;
            lastCmdTime = curTime;

            zoomCounter *= NUM_GRID_ROWS;
            
            if ((screenWidth/zoomCounter <= MOUSE_PRECISION)&&(screenHeight/zoomCounter <= MOUSE_PRECISION)) {
              if ((programMode == LEFT_CLICK_MODE)||(programMode == TYPE_MODE)){
                displayController->clickMouse(displayController->LEFT_CLICK);
              }
              zoomCounter = 1;
              programMode = IDLE_MODE;
            } else {
              zoomCounter++;
              displayController->zoom();
            }
          }
        
        // If both eyes are open
        } else {
          // Move mouse around
          displayController->warpMouse(focusPointX, curGridY);

          // If click command has recently been emitted, finish it
          if (clickCmd){
            // Empty buffers to prevent missinterpretations
            blinkLFilter->clear();
            blinkRFilter->clear();

            clickCmd = false;
          }
        }
      }
    }
  }
}
