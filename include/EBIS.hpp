/* TODO
Overall features:
1 - The drawn grid can have different colors depending on the color of the corresponding
 point on the image, so that grid is visible even on black background.

2 - Whenever user zooms in, there should be an animation, letting him know.
 This is necessary when zooming into a single colored image.

3 - Add other modes of operation:
 - Double click
 - Type
 - Right click
 - Alt + tab

4 - Faster screenshot acquisition.

5 - Save all build files on build folder using makefile

6 - Use a shadow removal algorithm to remove shadows from face

7 - Fix clicking, as XSendEvent isn't recognized by every window

Forever in debt (features that should always be made better):
1 - Head pose estimation.

2 - Eye contour. If the eyelids are captured perfectly, good for you!

3 - Blink detection. This is a pain!

4 - 3d estimation of face. Currently, only part of the points are calculated as 3d. 
 Why not the whole face?!

5 - Filtering. One can try using Kalman Filter, for example. This can improve head 
 pose estimation, pupil and eye region detection, and blink detection.

*/
#ifndef EBIS_HPP
#define EBIS_HPP

#include "opencv2/opencv.hpp"

#include <FaceTracker/Tracker.h>
#include <opencv/highgui.h>

#include <eyeLike/findEyeCenter.h>
#include <Flandmark.hpp>

#include <CommandHandler.hpp>

#include <Pong/Pong.hpp>

class Meye{
private:
  // Command handler
  CommandHandler commandHandler;

  // FPS variables
  int64 t1, t0; 
  int fnum;
  int fps;

  // Pong game!
  Pong *pong;

  // Detected focus point 
  cv::Point focusPoint;
  // Detected eye movement (not used)
  cv::Point eyeMovement;

  // Face Tracker variables
  FACETRACKER::Tracker *model;
  cv::Mat tri;
  cv::Mat con;
  std::vector<int> wSize1;
  std::vector<int> wSize2;
  int nIter;
  double clamp, fTol; 

  // Reset counter - FaceTracker is reset constantly to make sure the user is correctly detected
  int resetCounter;

  // Cascade classifiers
  cv::CascadeClassifier faceCascade, eyeCascade;

  // Detected faces vector
  std::vector<cv::Rect> faces;

  // Flandmarks
  Flandmark flandmark;

  // Facial landmarks of interest
  Landmarks landmarks;

  // Detection failure signal
  bool failed;

  // Screen size is updated at every call to update
  int screenWidth, screenHeight;

  // Find most likely point to be the center of the pupil in a image. 
  // Returns eye closure state: true if no pupil was found, that is, user is likely to have blinked
  bool findPupil(cv::Mat gray, cv::Rect eyeRegion, cv::Point& pupil);

  // Estimate gaze
  cv::Point calcFocusPoint(cv::Point pupilL, cv::Point pupilR, cv::Point3d headPosition, cv::Point3d headOrientation, 
                                cv::Point eyeCenterL, cv::Point eyeCenterR, cv::Point canthiLL, cv::Point canthiLR, 
                                cv::Point canthiRL, cv::Point canthiRR, int eyeROIWidth, cv::Mat debugImage);
  // Calculate relative eye movement
  cv::Point calcEyeMovement(cv::Point pupilL, cv::Point pupilR, cv::Point3d headPosition, cv::Point3d headOrientation, cv::Point eyeCenterL, cv::Point eyeCenterR);

  // Many methods to detect blink. 
  bool checkBlink(cv::Mat eyeGray, cv::Point &pupil);
  bool detectBlink(cv::Mat eyeImage, bool leftEye);

  // Calculate screen distance
  double calcScreenDistance();

  // Calculate head position
  cv::Point3d calcHeadPosition();

  // Calculate head orientation
  cv::Point3d calcHeadOrientation();

  // Calculate eye region (region containing the eye)
  cv::Rect calcEyeRegion(bool leftEye);

  // Apply CLAHE to equalize image histogram in order to raise contrast at night
  cv::Mat adaptativeHistEqualization(cv::Mat im);

  // Display frame rate
  void showFrameRate(cv::Mat image);

  // Draw FaceTracker landmarks
  void drawFace(cv::Mat &image,cv::Mat &shape,cv::Mat &con,cv::Mat &tri,cv::Mat &visi, cv::Point pupilL, cv::Point pupilR, cv::Point eyeCenterL, cv::Point eyeCenterR);
  // Display facial landmarks on given frame
  void displayLandmarks(cv::Mat& frame);

  // Map point from image size to screen size
  cv::Point mapToScreen(cv::Point p, cv::Size imageSize);
  cv::Point3d mapToScreen(cv::Point3d p, cv::Size imageSize);

  // Map point from screen size to image size
  cv::Point mapFromScreen(cv::Point p, cv::Size imageSize);
  cv::Point3d mapFromScreen(cv::Point3d p, cv::Size imageSize);

  // Calculate blink region (region containing the eye ONLY - no eyebrows/nose)
  cv::Rect calcBlinkRegion(bool leftEye);

  cv::Point detectGaze(cv::Mat image, bool &blinkLeft, bool &blinkRight);

public:
  Meye();
  ~Meye();

  // Main function - given an image, controls the entire system by identifying blinks and gaze direction
  void update(cv::Mat im);

  // Let's have some fun, shall we? Start Pong and use your eyes to move!
  void playPong(cv::Mat image);

  void playingWithHSV(cv::Mat im); // TODO remove
};

#endif