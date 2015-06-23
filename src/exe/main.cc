#include "EBIS.hpp"

#define FACE_CASCADE_PATH "res/haarcascade_frontalface_alt.xml"
#define EYE_CASCADE_PATH "res/haarcascade_eye_tree_eyeglasses.xml"
#define FLANDMARK_MODEL_PATH "res/flandmark_model.dat"

// Captures image data from camera and send to Meye class
void cameraCapture(Meye *meye){
  // Initialize camera
  cv::VideoCapture camera(0);
  if (!camera.isOpened()) return;

  // Exit on ESC
  while(cvWaitKey(10) != 27){
    // Grab image and flip
    cv::Mat image;
    camera >> image;
    cv::flip(image, image, 1);

    // THE call
    meye->update(image);
    //meye->playPong(image);
    //meye->playingWithHSV(image);
  }
}

// I'm the important guy, right?
int main(){
  Meye meye;

  cameraCapture(&meye);

  return 0;
}

/*
#ifdef __linux__
#include <dirent.h>

// Useful template for data precision test
// Unfortunately, it seems like the haarcascade_eye_tree_eyeglasses.xml 
// has been trained on these, so we get 100%
void closedEyeAnalysis(Meye *meye){
  int fp = 0, fn = 0, tp = 0, tn = 0;
  cv::Mat image;
  bool isBlink;

  DIR* dirp = opendir("../dataset_B_Eye_Images/closedLeftEyes");
  struct dirent *dp;
  // Check for closed left eyes
  while ((dp = readdir(dirp)) != NULL){
    std::string fileName = "../dataset_B_Eye_Images/closedLeftEyes/";
    image = cv::imread(fileName.append(dp->d_name));
    if (!image.data) continue;
    isBlink = meye->detectBlink(image);
    if (isBlink) tp++;
    else fn++;
  }
  closedir(dirp);

  dirp = opendir("../dataset_B_Eye_Images/closedRightEyes");
  // Check for closed right eyes
  while ((dp = readdir(dirp)) != NULL){
    std::string fileName = "../dataset_B_Eye_Images/closedRightEyes/";
    image = cv::imread(fileName.append(dp->d_name));
    if (!image.data) continue;
    isBlink = meye->detectBlink(image);
    if (isBlink) tp++;
    else fn++;
  }
  closedir(dirp);

  dirp = opendir("../dataset_B_Eye_Images/openLeftEyes");
  // Check for open left eyes
  while ((dp = readdir(dirp)) != NULL){
    std::string fileName = "../dataset_B_Eye_Images/openLeftEyes/";
    image = cv::imread(fileName.append(dp->d_name));
    if (!image.data) continue;
    isBlink = meye->detectBlink(image);
    if (isBlink) fp++;
    else tn++;
  }
  closedir(dirp);

  dirp = opendir("../dataset_B_Eye_Images/openRightEyes");
  // Check for open right eyes
  while ((dp = readdir(dirp)) != NULL){
    std::string fileName = "../dataset_B_Eye_Images/openRightEyes/";
    image = cv::imread(fileName.append(dp->d_name));
    if (!image.data) continue;
    isBlink = meye->detectBlink(image);
    if (isBlink) fp++;
    else tn++;
  }
  closedir(dirp);

  std::cout << "Total images analysed: " << fp + fn + tp + tn << std::endl;
  std::cout << "False Positives: " << fp << std::endl;
  std::cout << "False Negatives: " << fn << std::endl;
  std::cout << "True Positives: " << tp << std::endl;
  std::cout << "True Negatives: " << tn << std::endl;
}
#endif
*/
