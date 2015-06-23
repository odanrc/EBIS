#include <EBIS.hpp>

#include <iostream>

#define DEBUG_MODE

#define WINDOW_NAME "EBIS"

#define FACE_CASCADE_PATH "res/haarcascade_frontalface_alt.xml"
#define EYE_CASCADE_PATH "res/haarcascade_eye_tree_eyeglasses.xml"
#define FLANDMARK_MODEL_PATH "res/flandmark_model.dat"

#define FACETRACKER_FILE_PATH "model/face2.tracker"
#define FACETRACKER_CON_PATH "model/face.con"
#define FACETRACKER_TRI_PATH "model/face.tri"

// The average eye diameter is 24 mm
static const double EYE_DIAMETER = 2.4;

// System reset frequency
static const int RESET_FREQUENCY = 10;

//=============================================================================
// The points represent:
// 0 to 16, outline, from left to right
// 17 to 21, left earbrow, from left to right
// 22 to 26, right earbrow, from left to right
// 27 to 30, nose tip, top to bottom
// 31 to 35, nose base, left to right
// 36 to 41, left eye, clockwise starting from left
// 42 to 47, right eye, clockwise starting from left
// 48 to 59, outter mouth, clockwise starting from left
// 60 to 65, inner mouth, clockwise starting from left top
void Meye::drawFace(cv::Mat &image,cv::Mat &shape,cv::Mat &con,cv::Mat &tri,cv::Mat &visi, cv::Point pupilL, cv::Point pupilR, cv::Point eyeCenterL, cv::Point eyeCenterR){
  int i, n = shape.rows/2; cv::Point p1,p2;

  // Draw triangulation
  
  for(i = 0; i < tri.rows; i++){
    if(visi.at<int>(tri.at<int>(i,0),0) && visi.at<int>(tri.at<int>(i,1),0) && visi.at<int>(tri.at<int>(i,2),0)){
      p1 = cv::Point(shape.at<double>(tri.at<int>(i,0),0), shape.at<double>(tri.at<int>(i,0)+n,0));
      p2 = cv::Point(shape.at<double>(tri.at<int>(i,1),0), shape.at<double>(tri.at<int>(i,1)+n,0));
      cv::line(image,p1,p2,CV_RGB(0,0,0));
      p1 = cv::Point(shape.at<double>(tri.at<int>(i,0),0), shape.at<double>(tri.at<int>(i,0)+n,0));
      p2 = cv::Point(shape.at<double>(tri.at<int>(i,2),0), shape.at<double>(tri.at<int>(i,2)+n,0));
      cv::line(image,p1,p2,CV_RGB(0,0,0));
      p1 = cv::Point(shape.at<double>(tri.at<int>(i,2),0), shape.at<double>(tri.at<int>(i,2)+n,0));
      p2 = cv::Point(shape.at<double>(tri.at<int>(i,1),0), shape.at<double>(tri.at<int>(i,1)+n,0));
      cv::line(image,p1,p2,CV_RGB(0,0,0));
    }
  }
  // Draw connections
    for(i = 0; i < con.cols; i++){
    if(visi.at<int>(con.at<int>(0,i),0) && visi.at<int>(con.at<int>(1,i),0)){
      p1 = cv::Point(shape.at<double>(con.at<int>(0,i),0), shape.at<double>(con.at<int>(0,i)+n,0));
      p2 = cv::Point(shape.at<double>(con.at<int>(1,i),0), shape.at<double>(con.at<int>(1,i)+n,0));
      cv::line(image,p1,p2,CV_RGB(0,0,255),1);
    }
  }
  // Draw points
  for(i = 0; i < n; i++)
    if(visi.at<int>(i,0))
      cv::circle(image,cv::Point(shape.at<double>(i,0),shape.at<double>(i+n,0)),2,CV_RGB(255,0,0));

  // Draw pupils
  cv::circle(image, pupilL, 2, CV_RGB(255, 255, 255));
  cv::circle(image, pupilR, 2, CV_RGB(255, 255, 255));
}

void Meye::displayLandmarks(cv::Mat& frame){
  // Head center and nose
  cv::circle(frame, landmarks.center,     3, CV_RGB(0, 0, 255), CV_FILLED);
  cv::circle(frame, landmarks.nose,       3, CV_RGB(0, 255, 255), CV_FILLED);

  // Eye center
  //cv::circle(frame, landmarks.eyeCenterL, 3, CV_RGB(0, 0, 255), CV_FILLED);
  //cv::circle(frame, landmarks.eyeCenterR, 3, CV_RGB(0, 0, 255), CV_FILLED);

  // Pupil
  cv::circle(frame, landmarks.pupilL,     3, CV_RGB(255, 255, 255));
  cv::circle(frame, landmarks.pupilR,     3, CV_RGB(255, 255, 255));

  // Eye canthi
  cv::circle(frame, landmarks.canthiLL,   3, CV_RGB(255, 0, 0), CV_FILLED);
  cv::circle(frame, landmarks.canthiLR,   3, CV_RGB(255, 0, 0), CV_FILLED);
  cv::circle(frame, landmarks.canthiRL,   3, CV_RGB(255, 255, 0), CV_FILLED);
  cv::circle(frame, landmarks.canthiRR,   3, CV_RGB(255, 255, 0), CV_FILLED);

  // Mouth
  cv::circle(frame, landmarks.mouthL,     3, CV_RGB(255, 0, 255), CV_FILLED);
  cv::circle(frame, landmarks.mouthR,     3, CV_RGB(255, 0, 255), CV_FILLED);
}

bool Meye::findPupil(cv::Mat gray, cv::Rect eyeRegion, cv::Point& pupil){
  // Check if ROI is ok before sending, otherwise program will crash
  if ((eyeRegion.x < 0)||
    (eyeRegion.width < 0)||
    (eyeRegion.x + eyeRegion.width > gray.cols)||
    (eyeRegion.y < 0)||
    (eyeRegion.height < 0)||
    (eyeRegion.y + eyeRegion.height > gray.rows)
  ) return false;

  cv::Mat eyeROI = gray(eyeRegion);
  //cv::normalize(eyeROI, eyeROI, 0, 140, cv::NORM_MINMAX, CV_8UC1);

  // Find pupil
  pupil = findEyeCenter(eyeROI, cv::Rect(0, 0, eyeRegion.width, eyeRegion.height));

  // Change pupil center to screen coordinates
  pupil.x += eyeRegion.x;
  pupil.y += eyeRegion.y;

  return true;
}

cv::Point Meye::calcEyeMovement(cv::Point pupilL, cv::Point pupilR, cv::Point3d headPosition, cv::Point3d headOrientation, cv::Point eyeCenterL, cv::Point eyeCenterR){
  const int EYE_MOVEMENT_MULTIPLIER = 10;

  // Calculate distance of pupils from center of eyes
  cv::Point centerDistL(pupilL.x - eyeCenterL.x, pupilL.y - eyeCenterL.y);
  cv::Point centerDistR(pupilR.x - eyeCenterR.x, pupilR.y - eyeCenterR.y);

  cv::Point movement(((centerDistL.x+centerDistR.x)/2)*EYE_MOVEMENT_MULTIPLIER, ((centerDistL.y+centerDistR.y)/2)*EYE_MOVEMENT_MULTIPLIER);

  return movement;
}

bool Meye::checkBlink(cv::Mat eyeGray, cv::Point &pupil){
  double minVal = 0, maxVal = 0;
  bool isBlink = false;

  // Blur and normalize image
cv::imshow("Debug Left", eyeGray);
  cv::GaussianBlur(eyeGray, eyeGray, cv::Size(5, 5), 2, 2);
  cv::normalize(eyeGray, eyeGray, 0, 140, cv::NORM_MINMAX, CV_8UC1);
cv::imshow("Debug Right", eyeGray);

  // Find min color point (pupils or center of eye when eyes are closed)
  cv::minMaxLoc(eyeGray, &minVal, &maxVal, NULL, NULL);

  //cv::inRange(eyeGray, cv::Scalar(0), cv::Scalar(maxVal-10), eyeGray);

  // Isolate pupil
  cv::threshold(eyeGray, eyeGray, minVal + 10, 255, cv::THRESH_BINARY_INV);

  // Find contours
  std::vector<std::vector<cv::Point> > contours;
  cv::findContours(eyeGray, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

  cv::Mat im(eyeGray.rows, eyeGray.cols, eyeGray.type(), cv::Scalar(0));
  float maxRadius = 0;
  double maxArea;
  // Get circle of biggest radius
  for(size_t i = 0; i< contours.size(); i++) {
    cv::Point2f center;
    float radius;
    //drawContours(im, contours, i, cv::Scalar(255));

    cv::minEnclosingCircle(contours[i], center, radius);
    //cv::circle(im, center, (int)radius, cv::Scalar(255, 255, 0));

    if (radius > maxRadius){
      maxArea = cv::contourArea(contours[i]);
      pupil = center;
      maxRadius = radius;
    }
  }

  // If area of biggest circle is bigger than the area of the detected pupil * 3, it's a blink
  if (contours.size() > 0){
    cv::circle(eyeGray, pupil, (int)maxRadius, cv::Scalar(255, 255, 0));
    if (3.14*maxRadius*maxRadius > maxArea * 5) isBlink = true; 
  }

  return isBlink;
}

cv::Point3d Meye::calcHeadOrientation(){
  return cv::Point3d(model->_clm._pglobl.at<double>(1,0), 
                     model->_clm._pglobl.at<double>(2,0), 
                     model->_clm._pglobl.at<double>(3,0));
}

cv::Point3d Meye::calcHeadPosition(){
  return cv::Point3d(model->_clm._pglobl.at<double>(4,0), model->_clm._pglobl.at<double>(5,0), calcScreenDistance());
  //return cv::Point3d(faces[0].x+faces[0].width/2, faces[0].y+faces[0].height/2, calcScreenDistance());
}

cv::Rect Meye::calcEyeRegion(bool leftEye){
/*    int pointPos = leftEye ? 36 : 42;

  // Calculate eye region
  int minX = model->_shape.at<double>(pointPos, 0);
  int maxX = minX;
  int minY = model->_shape.at<double>(pointPos + 66, 0);
  int maxY = minY;
  for (int i = pointPos; i < pointPos+6; i++) {
    int valX = model->_shape.at<double>(i,0);
    int valY = model->_shape.at<double>(i+66,0);
    if (valX < minX) minX = valX;
    else if (valX > maxX) maxX = valX;
    if (valY < minY) minY = valY;
    else if (valY > maxY) maxY = valY;
  }
  // Workaround due to face tracker problem
  maxX  += (maxX-minX)/10;

  int errorMargin = (maxX-minX)/4;
  minX  -= errorMargin;
  maxX  += errorMargin;
  minY  -= errorMargin;
  maxY  += errorMargin;

  return cv::Rect(minX, minY, maxX - minX, maxY - minY);
  */
  if (leftEye) return cv::Rect(landmarks.canthiLL.x, landmarks.canthiLL.y - (landmarks.canthiLR.x-landmarks.canthiLL.x)*0.4, 
                       landmarks.canthiLR.x-landmarks.canthiLL.x, (landmarks.canthiLR.x-landmarks.canthiLL.x)*0.8);
  else return cv::Rect(landmarks.canthiRL.x, landmarks.canthiRL.y - (landmarks.canthiRR.x-landmarks.canthiRL.x)*0.4, 
                       landmarks.canthiRR.x-landmarks.canthiRL.x, (landmarks.canthiRR.x-landmarks.canthiRL.x)*0.8);
}

cv::Rect Meye::calcBlinkRegion(bool leftEye){
  int highX, highY, lowX, lowY, width;
  if (leftEye){
    highX = std::max(landmarks.canthiLL.x, landmarks.canthiLR.x);
    lowX = std::min(landmarks.canthiLL.x, landmarks.canthiLR.x);

    highY = std::max(landmarks.canthiLL.y, landmarks.canthiLR.y);
    lowY = std::min(landmarks.canthiLL.y, landmarks.canthiLR.y);
  } else {
    highX = std::max(landmarks.canthiRL.x, landmarks.canthiRR.x);
    lowX = std::min(landmarks.canthiRL.x, landmarks.canthiRR.x);

    highY = std::max(landmarks.canthiRL.y, landmarks.canthiRR.y);
    lowY = std::min(landmarks.canthiRL.y, landmarks.canthiRR.y);
  }

  width = highX - lowX;

  cv::Rect r = cv::Rect(lowX - width,
                  lowY - width,
                  width*3,
                  width*3);

  return r;
}
double euclidianDistance(cv::Point p1, cv::Point p2){
  return sqrt((p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y));
}

cv::Point Meye::mapToScreen(cv::Point p, cv::Size imageSize){
  cv::Point mapP;
  mapP.x = p.x*screenWidth/imageSize.width;
  mapP.y = p.y*screenHeight/imageSize.height;
  return mapP;
}

cv::Point3d Meye::mapToScreen(cv::Point3d p, cv::Size imageSize){
  cv::Point3d mapP;
  mapP.x = p.x*screenWidth/imageSize.width;
  mapP.y = p.y*screenHeight/imageSize.height;
  mapP.z = p.z;
  return mapP;
}

cv::Point Meye::mapFromScreen(cv::Point p, cv::Size imageSize){
  cv::Point mapP;
  mapP.x = p.x*imageSize.width/screenWidth;
  mapP.y = p.y*imageSize.height/screenHeight;
  return mapP;
}

cv::Point3d Meye::mapFromScreen(cv::Point3d p, cv::Size imageSize){
  cv::Point3d mapP;
  mapP.x = p.x*imageSize.width/screenWidth;
  mapP.y = p.y*imageSize.height/screenHeight;
  mapP.z = p.z;
  return mapP;
}

// TODO calibrate based on person
double Meye::calcScreenDistance(){
  return screenWidth*270/faces[0].width;
}

// This will only rotate the back of the eye according to the head orientation
// This works, but could be better
cv::Point Meye::calcFocusPoint(cv::Point pupilL, cv::Point pupilR, cv::Point3d headPosition, cv::Point3d headOrientation, 
                                cv::Point eyeCenterL, cv::Point eyeCenterR, cv::Point canthiLL, cv::Point canthiLR, 
                                cv::Point canthiRL, cv::Point canthiRR, int eyeROIWidth, cv::Mat debugImage){
  // Estimate that eye diameter in video coordinates is the same as the distance between canthii
  cv::Matx13d eyeBackL(0, 0, euclidianDistance(canthiLR,canthiLL)/2), eyeBackR(0, 0, euclidianDistance(canthiRR,canthiRL)/2);

  // Calculate rotation matrix
  cv::Matx33d rotX(1, 0, 0,
              0, cos(headOrientation.x), -sin(headOrientation.x),
              0, sin(headOrientation.x), cos(headOrientation.x));
  cv::Matx33d rotY(cos(headOrientation.y), 0, sin(headOrientation.y),
              0, 1, 0,
              -sin(headOrientation.y), 0, cos(headOrientation.y));
  cv::Matx33d rotZ(cos(headOrientation.z), -sin(headOrientation.z), 0,
              sin(headOrientation.z), cos(headOrientation.z), 0,
              0, 0, 1);
  cv::Matx33d headRot = rotZ*rotY*rotX;

  eyeBackL = eyeBackL * -headRot;
  eyeBackR = eyeBackR * -headRot;

  eyeBackL(0,0) += eyeCenterL.x;
  eyeBackL(0,1) += eyeCenterL.y;  
  eyeBackR(0,0) += eyeCenterR.x;
  eyeBackR(0,1) += eyeCenterR.y;

  // Debug
  //cv::circle(debugImage, mapFromScreen(cv::Point(eyeBackL(0,0), eyeBackL(0,1)), debugImage.size()), 2, CV_RGB(255, 0, 255));
  //cv::circle(debugImage, mapFromScreen(cv::Point(eyeBackR(0,0), eyeBackR(0,1)), debugImage.size()), 2, CV_RGB(255, 0, 255));

  // Calculate distance of pupil from center of eye sphere
  cv::Point centerDistL(pupilL.x - eyeBackL(0,0), pupilL.y - eyeBackL(0,1));
  cv::Point centerDistR(pupilR.x - eyeBackR(0,0), pupilR.y - eyeBackR(0,1));

  // Calculate pupil angle relative to center of eye sphere
  double angleHorL = atan(centerDistL.x/eyeBackL(0,2));
  double angleVerL = atan(centerDistL.y/eyeBackL(0,2));
  double angleHorR = atan(centerDistR.x/eyeBackR(0,2));
  double angleVerR = atan(centerDistR.y/eyeBackR(0,2));

  // Calculate screen focus position
  // TODO it's still wrong, because the rotation of the face increases the distance from the screen 
  // of one of the sides of the face, while decreasing from the other.
  cv::Point screenPosL = cv::Point(eyeBackL(0,0) - tan(angleHorL)*(headPosition.z + eyeBackL(0,2)), 
                                   eyeBackL(0,1) - tan(angleVerL)*(headPosition.z + eyeBackL(0,2)));
  cv::Point screenPosR = cv::Point(eyeBackR(0,0) - tan(angleHorR)*(headPosition.z + eyeBackR(0,2)), 
                                   eyeBackR(0,1) - tan(angleVerR)*(headPosition.z + eyeBackR(0,2)));

  // Calculate screen position
  cv::Point screenPos((screenPosL.x + screenPosR.x)/2, (screenPosL.y + screenPosR.y)/2);

  // Wrap coordinates to valid points
  if      (screenPos.x <            0) screenPos.x = 0;
  else if (screenPos.x >  screenWidth) screenPos.x = screenWidth;
  if      (screenPos.y <            0) screenPos.y = 0;
  else if (screenPos.y > screenHeight) screenPos.y = screenHeight;

  /*
  // Draw gaze lines
  cv::line(debugImage, 
           mapFromScreen(cv::Point(eyeBackL(0, 0), eyeBackL(0, 1)), 
           debugImage.size()),
           mapFromScreen(cv::Point(pupilL.x + (pupilL.x-eyeBackL(0,0))*4, 
                                   pupilL.y + (pupilL.y-eyeBackL(0,1))*4), 
                         debugImage.size()), 
           CV_RGB(255, 255, 0));
  cv::line(debugImage, 
           mapFromScreen(cv::Point(eyeBackR(0, 0), eyeBackR(0, 1)), 
                         debugImage.size()), 
           mapFromScreen(cv::Point(pupilR.x + (pupilR.x-eyeBackR(0,0))*4, 
                                   pupilR.y + (pupilR.y-eyeBackR(0,1))*4), 
                         debugImage.size()), 
           CV_RGB(255, 255, 0));
  */

  return screenPos;
}

/*
  Detects if a blink happened by using a HAAR Classifier.
  The blink is then confirmmed by checkBlink, and, if both are true, a blink happened.
*/
bool Meye::detectBlink(cv::Mat eyeImage, bool leftEye){
  std::vector<cv::Rect> eyes;

  // Normalize image for better results  
  //cv::normalize(eyeImage, eyeImage, 0, 100, cv::NORM_MINMAX, CV_8UC1);

  // Detect eyes
  eyeCascade.detectMultiScale(eyeImage, eyes, 1.05, 3, 0|CV_HAAR_SCALE_IMAGE|CV_HAAR_FIND_BIGGEST_OBJECT, cv::Size(10, 10));
  //if (eyes.size() > 0)
    //if (leftEye) cv::imshow("Debug Left", eyeImage(eyes[0]));
    //else cv::imshow("Debug Right", eyeImage(eyes[0]));

  // If no eyes were detected, a blink is likely to have occurred
  return eyes.size() == 0;
}

cv::Point Meye::detectGaze(cv::Mat im, bool &blinkLeft, bool &blinkRight){
  // Pupil center
  cv::Point pupilL, pupilR;

  // Eye center
  cv::Point eyeCenterL, eyeCenterR;

  // Get gray image
  cv::Mat gray;
  cv::cvtColor(im, gray, CV_BGR2GRAY);

  if (!failed) {
    // Get eye region
    cv::Rect eyeROIL = calcEyeRegion(true);
    cv::Rect eyeROIR = calcEyeRegion(false);

    // TODO remove - find pupils using HOG
/*    findPupil(gray, eyeROIL, pupilL);
    findPupil(gray, eyeROIR, pupilR);

    landmarks.pupilL = pupilL;
    landmarks.pupilR = pupilR;
*/

    // Get head position and orientation
    cv::Point3d headOrientation = calcHeadOrientation();
    cv::Point3d headPosition    = calcHeadPosition();

    // Calculate eye centers
    //eyeCenterL = cv::Point(eyeROIL.x+eyeROIL.width/2, eyeROIL.y+eyeROIL.height/2);
    //eyeCenterR = cv::Point(eyeROIR.x+eyeROIR.width/2, eyeROIR.y+eyeROIR.height/2);

    // Check for blinks
    bool leftEyeBlink = false, rightEyeBlink = false;
    bool blinkL, blinkR, blinkDetectedL, blinkDetectedR;
    cv::Mat blinkIm1;
    gray.copyTo(blinkIm1);

cv::Point pupil2, pupil3;
    blinkL = checkBlink(blinkIm1(eyeROIL), pupilL);
    blinkR = checkBlink(blinkIm1(eyeROIR), pupilR);

    // Map pupil position to image
    pupilL.x += eyeROIL.x;
    pupilL.y += eyeROIL.y;
    pupilR.x += eyeROIR.x;
    pupilR.y += eyeROIR.y;

    landmarks.pupilL = pupilL;
    landmarks.pupilR = pupilR;

    //lineBlinkDetector(im(calcBlinkRegion(false)), false);

    blinkDetectedL = detectBlink(gray(calcBlinkRegion(true)), true);
    blinkDetectedR = detectBlink(gray(calcBlinkRegion(false)), false);

    // Estimate point on screen being looked at
    if (!(blinkL||blinkR))
      focusPoint = calcFocusPoint(mapToScreen(pupilL, im.size()), 
                                    mapToScreen(pupilR, im.size()),
                                    mapToScreen(headPosition, im.size()),
                                    headOrientation,
                                    mapToScreen(landmarks.eyeCenterL, im.size()),
                                    mapToScreen(landmarks.eyeCenterR, im.size()),
                                    mapToScreen(landmarks.canthiLL, im.size()),
                                    mapToScreen(landmarks.canthiLR, im.size()),
                                    mapToScreen(landmarks.canthiRL, im.size()),
                                    mapToScreen(landmarks.canthiRR, im.size()),
                                    eyeROIR.width,
                                    im);

    // Calculate eye movement
/*    if (!(blinkL||blinkR))
      eyeMovement = calcEyeMovement(pupilL, pupilR, 
                    headPosition, headOrientation, 
                    eyeCenterL, eyeCenterR);
*/

    blinkLeft = blinkDetectedL && blinkL;
    blinkRight = blinkDetectedR && blinkR;
  }

  return focusPoint;
}

void Meye::update(cv::Mat im){
  bool blinkL, blinkR;
  cv::Point focusPoint;

  cv::Rect face;

  // Get current desktop resolution
  commandHandler.getWindowSize(screenWidth, screenHeight);

  // TODO only use this at night
/*  cv::GaussianBlur(im, im, cv::Size(5, 5), 2, 2);
  cv::normalize(im, im, 0, 100, cv::NORM_MINMAX, CV_8UC1);
  im = adaptativeHistEqualization(im);
*/
  // Get gray image
  cv::Mat gray;
  cv::cvtColor(im, gray, CV_BGR2GRAY);

  // Get current desktop resolution
  commandHandler.getWindowSize(screenWidth, screenHeight);

  // Track face
  failed = model->Track(gray, failed ? wSize2 : wSize1, -1, nIter, clamp, fTol, false);
  // Reset model on failure
  if (failed) model->FrameReset();
  else {
    faceCascade.detectMultiScale(gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE|CV_HAAR_FIND_BIGGEST_OBJECT, cv::Size(150, 150));
    failed = faces.size() == 0;
    if (!failed){
      face = faces[0];
      IplImage grayFrame = gray;
      flandmark.detect(&grayFrame, &face);
      flandmark.getLandmarks(landmarks);

      focusPoint = detectGaze(im, blinkL, blinkR);
    }
  }

  #ifdef DEBUG_MODE
    // Show framerate
    showFrameRate(im);

    // Draw face
    if (failed) cv::rectangle(im, cv::Rect(0, 0, 150, 150), cv::Scalar(0, 0, 255), CV_FILLED);
    else {
      //drawFace(im, model->_shape, con, tri, model->_clm._visi[model->_clm.GetViewIdx()], 
      //         pupilL, pupilR, eyeCenterL, eyeCenterR); 
      displayLandmarks(im);
    }

    // Show image
    cv::imshow(WINDOW_NAME, im); 
  #endif

  if (!failed){
    // Parse command
    commandHandler.parse(blinkL, blinkR, focusPoint.x, focusPoint.y);
  }
}

void Meye::playPong(cv::Mat image){
  commandHandler.getWindowSize(screenWidth, screenHeight);

  // We haven't started playing, so let's play!
  if (!pong || (pong->isGameOver() && (cvWaitKey(20) == 'r'))){
    std::cout << "R" << std::endl;
    pong = new Pong(screenWidth, screenHeight);

    // Start playing!
    pong->start();
  } 

  cv::Point focusPoint;

  cv::Rect face;

  // Get gray image
  cv::Mat gray;
  cv::cvtColor(image, gray, CV_BGR2GRAY);

  // Track face
  failed = model->Track(gray, failed ? wSize2 : wSize1, -1, nIter, clamp, fTol, false);
  // Reset model on failure
  if (failed) model->FrameReset();
  else {
    faceCascade.detectMultiScale(gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE|CV_HAAR_FIND_BIGGEST_OBJECT, cv::Size(150, 150));
    failed = faces.size() == 0;
    if (!failed){
      face = faces[0];
      IplImage grayFrame = gray;
      flandmark.detect(&grayFrame, &face);
      flandmark.getLandmarks(landmarks);

      bool blinkL, blinkR;
      focusPoint = detectGaze(image, blinkL, blinkR);
    }
  }

  pong->update();

  // Move towards gaze direction
  if (!failed) pong->moveTo(focusPoint.x);
}

void Meye::showFrameRate(cv::Mat image){
  char fpsText[256];

  // Draw framerate on display image 
  if (++fnum >= 10){
    t1 = cvGetTickCount();
    fps = 10.0/((double(t1-t0)/cvGetTickFrequency())/1e+6); 
    t0 = t1;
    fnum = 0;

    // Reset model in order to avoid errors every RESET_FREQUENCY*fps frames
    if (++resetCounter == RESET_FREQUENCY){
      resetCounter = 0;
      model->FrameReset();
    }
  }

  sprintf(fpsText, "%d frames/sec", fps);
  cv::putText(image, fpsText, cv::Point(10,20), CV_FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(255,255,255));
}

Meye::Meye(){
  resetCounter = 0;

  //set other tracking parameters
  wSize1.push_back(7);
  wSize2.push_back(11);
  wSize2.push_back(9);
  wSize2.push_back(7);
  nIter = 5;
  clamp = 3;
  fTol = 0.01; 

  // Initialize tracker
  model = new FACETRACKER::Tracker(FACETRACKER_FILE_PATH);
  tri = FACETRACKER::IO::LoadTri(FACETRACKER_TRI_PATH);
  con = FACETRACKER::IO::LoadCon(FACETRACKER_CON_PATH);

  // Load face cascade
  if (!faceCascade.load(FACE_CASCADE_PATH)) throw "Could not load face cascade."; 
  if (!eyeCascade.load(EYE_CASCADE_PATH)) throw "Could not load eye cascade."; 

  // Init Flandmark
  try { flandmark.init(FLANDMARK_MODEL_PATH); }
  catch (std::exception e) { throw e; }

  // FPS variables
  t0 = cvGetTickCount();
  fnum = 0;
  fps = 0;
  
  // Create window
  #ifdef DEBUG_MODE
    cv::namedWindow(WINDOW_NAME, CV_WINDOW_FREERATIO);
    cv::namedWindow("Debug Left", CV_WINDOW_FREERATIO);
    cv::namedWindow("Debug Right", CV_WINDOW_FREERATIO);
    //cv::namedWindow("Debug Gray", CV_WINDOW_FREERATIO);
    //cv::namedWindow("Debug V", CV_WINDOW_FREERATIO);
    //cv::namedWindow("Debug S", CV_WINDOW_FREERATIO);
  #endif

  failed = true;

  focusPoint = cv::Point(0, 0);
  eyeMovement = cv::Point(0, 0);

  pong = NULL;
}

Meye::~Meye(){
  cv::destroyWindow(WINDOW_NAME);
}

cv::Mat Meye::adaptativeHistEqualization(cv::Mat im){
  cv::Mat lab_image;
  cv::cvtColor(im, lab_image, CV_BGR2Lab);

  // Extract the L channel
  std::vector<cv::Mat> lab_planes(3);
  cv::split(lab_image, lab_planes);  // now we have the L image in lab_planes[0]

  // Apply the CLAHE algorithm to the L channel
  cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
  clahe->setClipLimit(4);
  cv::Mat dst;
  clahe->apply(lab_planes[0], dst);

  // Merge the the color planes back into an Lab image
  dst.copyTo(lab_planes[0]);
  cv::merge(lab_planes, lab_image);

  cv::Mat image_clahe;
  cv::cvtColor(lab_image, image_clahe, CV_Lab2BGR);

  cv::imshow("Clahe", image_clahe);

  return image_clahe;
}

// TODO remove
void Meye::playingWithHSV(cv::Mat im){
  cv::Rect face;

  // Pupil center
  cv::Point pupilL, pupilR;

  // Eye center
  cv::Point eyeCenterL, eyeCenterR;

  cv::Mat imCopy;
  im.copyTo(imCopy);

  cv::GaussianBlur(im, im, cv::Size(5, 5), 2, 2);
  cv::normalize(im, im, 0, 100, cv::NORM_MINMAX, CV_8UC1);
  // TODO only use this at night
  im = adaptativeHistEqualization(im);

  // Get gray image
  cv::Mat gray;
  cv::cvtColor(im, gray, CV_BGR2GRAY);

  // Get hsv image
  cv::Mat hsv, yuv;
  cv::Mat splHSV[3], splYUV[3];
  cv::cvtColor(im, hsv, CV_BGR2HSV);
  cv::cvtColor(im, yuv, CV_BGR2YUV);
  cv::split(hsv, splHSV);
  cv::split(yuv, splYUV);
  imshow("g", splYUV[0]);

  // Get current desktop resolution
  commandHandler.getWindowSize(screenWidth, screenHeight);

  // Track face
  faceCascade.detectMultiScale(gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE|CV_HAAR_FIND_BIGGEST_OBJECT, cv::Size(150, 150));
  failed = faces.size() == 0;
  if (!failed){
    face = faces[0];
    IplImage grayFrame = gray;
    flandmark.detect(&grayFrame, &face);
    flandmark.getLandmarks(landmarks);

    // Get eye region
    cv::Rect eyeROIL = calcEyeRegion(true);
    cv::Rect eyeROIR = calcEyeRegion(false);

    splHSV[0] = splHSV[0](eyeROIL);
    splHSV[1] = splHSV[1](eyeROIL);
    splHSV[2] = splHSV[2](eyeROIL);
    gray = gray(eyeROIL);
    cv::Mat originalGray;

    cv::GaussianBlur(gray, gray, cv::Size(5, 5), 2, 2);
    cv::normalize(gray, gray, 0, 100, cv::NORM_MINMAX, CV_8UC1);

    gray.copyTo(originalGray);

    gray = splHSV[2]; // TODO WATCHOUT
    for (int i = 0; i < 3; i++){
      cv::GaussianBlur(splHSV[i], splHSV[i], cv::Size(5, 5), 2, 2);
      cv::normalize(splHSV[i], splHSV[i], 0, 100, cv::NORM_MINMAX, CV_8UC1);
    }

    double minVal = 0, maxVal = 0;
    cv::minMaxLoc(splHSV[0], &minVal, &maxVal, NULL, NULL);
    cv::threshold(splHSV[0], splHSV[0], (maxVal+minVal)/2, 20, cv::THRESH_BINARY);

    splHSV[1] = cv::Scalar::all(255) - splHSV[1];

    cv::imshow("Debug V", splHSV[2]);
    cv::moveWindow("Debug V", 0, 100);
    cv::imshow("Debug S", splHSV[1]);
    cv::moveWindow("Debug S", 500, 100);
    cv::imshow("Debug H", splHSV[0]);
    cv::moveWindow("Debug H", 1000, 100);
    cv::imshow("Gray", gray);
    cv::moveWindow("Gray", 0, 500);

    cv::Mat opImage;
    //cv::bitwise_and(splHSV[2], splHSV[0], opImage);
    //cv::addWeighted(splHSV[2], alpha, splHSV[0], 1-alpha, 0.0, opImage);
    add(splHSV[2], splHSV[2], opImage);
    //cv::normalize(opImage, opImage, 0, 100, cv::NORM_MINMAX, CV_8UC1);
    for (int i = 0; i < 100; i++){

      add(opImage, opImage, opImage);
      cv::minMaxLoc(opImage, &minVal, &maxVal, NULL, NULL);
      if (maxVal > 125)
      cv::normalize(opImage, opImage, 0, 50, cv::NORM_MINMAX, CV_8UC1);
    }
    cv::normalize(opImage, opImage, 0, 100, cv::NORM_MINMAX, CV_8UC1);
    cv::imshow("Op", opImage);
    cv::moveWindow("Op", 500, 500);

    cv::minMaxLoc(opImage, &minVal, &maxVal, NULL, NULL);
    cv::threshold(opImage, opImage, (minVal+maxVal)/4, 255, cv::THRESH_BINARY_INV);
    //cv::GaussianBlur(opImage, opImage, cv::Size(9, 9), 2, 2);
    //cv::minMaxLoc(opImage, &minVal, &maxVal, NULL, NULL);
    //cv::threshold(opImage, opImage, ((minVal+maxVal)*7)/8, 255, cv::THRESH_BINARY_INV);
    cv::imshow("Pupil", opImage);
    cv::moveWindow("Pupil", 1000, 500);

    cv::minMaxLoc(originalGray, &minVal, &maxVal, NULL, NULL);
    cv::threshold(originalGray, originalGray, (minVal+maxVal)/8, 255, cv::THRESH_BINARY_INV);
    cv::imshow("Original Pupil", originalGray);
    cv::moveWindow("Original Pupil", 1000, 700);
  }

  // Draw face - TODO remove
  if (failed) cv::rectangle(im, cv::Rect(0, 0, 150, 150), cv::Scalar(0, 0, 255), CV_FILLED);
  else displayLandmarks(im);

  // Show image
  cv::imshow(WINDOW_NAME, imCopy); 
}
