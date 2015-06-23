#ifndef COMMAND_HANDLER_HPP
#define COMMAND_HANDLER_HPP

#include <DisplayControl/DisplayController.hpp>
#include <SmoothingFilter.hpp>

#include <ctime>

class CommandHandler{
private:
  // Display controller
  DisplayController *displayController;

  // Time counter - a command can only be sent after 1 second has passed from last command
  time_t lastCmdTime;

  // Program execution modes
  typedef enum PROGRAM_MODE{IDLE_MODE, LEFT_CLICK_MODE, SCROLL_MODE, TYPE_MODE, RIGHT_CLICK_MODE, DOUBLE_CLICK_MODE, ALT_TAB_MODE} PROGRAM_MODE;
  PROGRAM_MODE programMode;

  // Gaze position filters
  SmoothingFilter *xFilter, *yFilter;
  // Blink filters
  SmoothingFilter *blinkLFilter, *blinkRFilter;

  bool modeCmd, clickCmd;
  // Counts the number of maginifications
  int zoomCounter;

  // Holds current grid Y position
  int curGridY;

  void switchMode(int screenWidth, int screenHeight);

public:
  CommandHandler();
  ~CommandHandler();

  // Get window size
  void getWindowSize(int &width, int &height);

  // Parses the given command
  void parse(bool leftEyeBlink, bool rightEyeBlink, int focusPointX, int focusPointY);
};

#endif