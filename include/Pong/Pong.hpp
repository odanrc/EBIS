#ifndef PONG_HPP
#define PONG_HPP

#include <iostream>
#include <cstdlib>

#include <opencv/highgui.h>

class Pong{
  private:
    int playerWidth;

    int p1x, p2x;
    int p1DestX, p2DestX;

    enum MOVEMENT {FORWARD, LEFT31, LEFT21, LEFT11, LEFT12, LEFT13, RIGHT13, RIGHT12, RIGHT11, RIGHT21, RIGHT31};

    // Number of times the ball hit a player
    int numberHits;

    cv::Mat image;

    struct ball{
      int x;
      int y;
      int dir;
      int mov;
    } ball;

    // If set, game ends
    bool gameOver;

    // Screen width and height
    int width, height;

    // Movement functions
    void moveBall();
    void moveOpponent();

    // Move player to stored location, in case fps is low
    void move();

    // Render scene
    void draw();

    // Ball collision checkers. The movement of the ball is changed if a collision happens.
    void checkWallCollision();
    void checkPlayerCollision();

    // Checks for game over
    void checkEndGame();

    Pong(){}
  public:
    Pong(int width, int height);
    ~Pong();

    // Set new player destination
    void moveTo(int x);

    // Move player
    void move(int x);

    // Start game
    void start();

    // Checks for movement, collision, render scene and check for game end
    void update();

    // Returns if the game is over
    bool isGameOver();
};

#endif