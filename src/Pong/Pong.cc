#include <Pong/Pong.hpp>

static const char* WINDOW_NAME = "Pong";

static const char* WIN_MESSAGE = "Voce venceu!";
static const char* LOSE_MESSAGE = "Olho na bola!";

static const int PLAYER_VELOCITY = 10;
static int PLAYER_1_Y;
static const int PLAYER_2_Y = 10;

static const int NUM_DIRECTIONS = 11;

static const int DEFAULT_PLAYER_WIDTH = 200;
static const int DEFAULT_PLAYER_HEIGHT = 15;
static const int BALL_RADIUS = 10;

static const int AI_VELOCITY_DECREASE_TIME = 5;

Pong::Pong(int width, int height){
  gameOver = true;

  this->width = width;
  this->height = height;

  playerWidth = std::min(DEFAULT_PLAYER_WIDTH, width/6);

  PLAYER_1_Y = height - DEFAULT_PLAYER_HEIGHT - PLAYER_2_Y;

  cv::namedWindow(WINDOW_NAME, CV_WINDOW_NORMAL);
  cv::setWindowProperty(WINDOW_NAME, CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);

  start();
}

Pong::~Pong(){
  cv::destroyWindow(WINDOW_NAME);
}

void Pong::moveBall(){
  switch(ball.mov){
    case FORWARD:
      ball.x += 0;
      ball.y += 2*ball.dir*2;
      break;
    case RIGHT31:
      ball.x += 3;
      ball.y += 1*ball.dir*2;
      break;
    case RIGHT21:
      ball.x += 2;
      ball.y += 1*ball.dir*2;
      break;
    case RIGHT11:
      ball.x += 1;
      ball.y += 1*ball.dir*2;
      break;
    case RIGHT12:
      ball.x += 1;
      ball.y += 2*ball.dir*2;
      break;
    case RIGHT13:
      ball.x += 1;
      ball.y += 3*ball.dir*2;
      break;
    case LEFT31:
      ball.x -= 3;
      ball.y += 1*ball.dir*2;
      break;
    case LEFT21:
      ball.x -= 2;
      ball.y += 1*ball.dir*2;
      break;
    case LEFT11:
      ball.x -= 1;
      ball.y += 1*ball.dir*2;
      break;
    case LEFT12:
      ball.x -= 1;
      ball.y += 2*ball.dir*2;
      break;
    case LEFT13:
      ball.x -= 1;
      ball.y += 3*ball.dir*2;
      break;
  }
}

void Pong::moveOpponent(){
  p2x = p2DestX;

  p2DestX += ((ball.x >= p2x)?rand()%PLAYER_VELOCITY/(2+numberHits/AI_VELOCITY_DECREASE_TIME):-rand()%PLAYER_VELOCITY/(2+numberHits/AI_VELOCITY_DECREASE_TIME));

  // Randomly move opponent
  if (p2DestX >= width - playerWidth/2) p2DestX = width - playerWidth/2;
  else if (p2DestX <= playerWidth/2) p2DestX = playerWidth/2;
}

void Pong::draw(){
  image = cv::Mat(height, width, CV_8UC3, cv::Scalar(0,0,0));

  // Draw player 1
  cv::rectangle(image, cv::Point(p1x - playerWidth/2, PLAYER_1_Y), cv::Point(p1x + playerWidth/2, PLAYER_1_Y + DEFAULT_PLAYER_HEIGHT), cv::Scalar(255, 255, 255));

  // Draw player 2
  cv::rectangle(image, cv::Point(p2x - playerWidth/2, PLAYER_2_Y), cv::Point(p2x + playerWidth/2, PLAYER_2_Y + DEFAULT_PLAYER_HEIGHT), cv::Scalar(255, 255, 255));

  // Draw ball
  cv::circle(image, cv::Point(ball.x, ball.y), BALL_RADIUS, cv::Scalar(255, 255, 255));

  cv::imshow(WINDOW_NAME, image);
  cv::moveWindow(WINDOW_NAME, 0, 0);
}

void Pong::checkWallCollision(){
  if (ball.x >= width - BALL_RADIUS){
    switch(ball.mov){
      case RIGHT31:
        ball.mov = LEFT31;
        break;
      case RIGHT21:
        ball.mov = LEFT21;
        break;
      case RIGHT11:
        ball.mov = LEFT11;
        break;
      case RIGHT12:
        ball.mov = LEFT12;
        break;
      case RIGHT13:
        ball.mov = LEFT13;
        break;
    }
  }
  if (ball.x <= BALL_RADIUS){
    switch(ball.mov){
      case LEFT31:
        ball.mov = RIGHT31;
        break;
      case LEFT21:
        ball.mov = RIGHT21;
        break;
      case LEFT11:
        ball.mov = RIGHT11;
        break;
      case LEFT12:
        ball.mov = RIGHT12;
        break;
      case LEFT13:
        ball.mov = RIGHT13;
        break;
    }
  }
}

void Pong::checkPlayerCollision(){
  bool collided = false;
  bool moving = false;
  bool movingRight = false;

  if ((ball.y + BALL_RADIUS >= PLAYER_1_Y) && (ball.y + BALL_RADIUS <= PLAYER_1_Y + 6)&&(ball.x >= p1x - playerWidth/2)&&(ball.x <= p1x + playerWidth/2)){
    ball.dir *= -1;

    ball.y = PLAYER_1_Y - BALL_RADIUS - 1;

    collided = true;
    moving = p1DestX != p1x;
    movingRight = p1DestX > p1x;

  } else if ((ball.y - BALL_RADIUS <= PLAYER_2_Y) && (ball.y - BALL_RADIUS >= PLAYER_2_Y - 6)&&(ball.x >= p2x - playerWidth/2)&&(ball.x <= p2x + playerWidth/2)){
    ball.dir *= -1;

    ball.y = PLAYER_2_Y + BALL_RADIUS + 1;

    collided = true;
    moving = p2DestX != p2x;
    movingRight = p2DestX > p2x;
  }

  // If there's a collision, change ball movement
  if (collided){
    numberHits++;

    // If player is moving right
    if (moving){
      if (movingRight){
        switch(ball.mov){
          case FORWARD:
            ball.mov = LEFT13;
            break;
          case RIGHT31:
            ball.mov = RIGHT21;
            break;
          case RIGHT21:
            ball.mov = RIGHT11;
            break;
          case RIGHT11:
            ball.mov = RIGHT12;
            break;
          case RIGHT12:
            ball.mov = RIGHT13;
            break;
          case RIGHT13:
            ball.mov = FORWARD;
            break;
          case LEFT31:
            ball.mov = LEFT31;
            break;
          case LEFT21:
            ball.mov = LEFT31;
            break;
          case LEFT11:
            ball.mov = LEFT21;
            break;
          case LEFT12:
            ball.mov = LEFT11;
            break;
          case LEFT13:
            ball.mov = LEFT12;
            break;
        }
      } else {
        switch(ball.mov){
          case FORWARD:
            ball.mov = RIGHT13;
            break;
          case RIGHT31:
            ball.mov = RIGHT31;
            break;
          case RIGHT21:
            ball.mov = RIGHT31;
            break;
          case RIGHT11:
            ball.mov = RIGHT21;
            break;
          case RIGHT12:
            ball.mov = RIGHT11;
            break;
          case RIGHT13:
            ball.mov = RIGHT12;
            break;
          case LEFT31:
            ball.mov = LEFT21;
            break;
          case LEFT21:
            ball.mov = LEFT11;
            break;
          case LEFT11:
            ball.mov = LEFT12;
            break;
          case LEFT12:
            ball.mov = LEFT13;
            break;
          case LEFT13:
            ball.mov = FORWARD;
            break;
        }     
      }
    }
  }
}

void Pong::checkEndGame(){
  std::string message;
  if (ball.y > PLAYER_1_Y) {
    message = LOSE_MESSAGE;

    gameOver = true;
  } else if (ball.y < PLAYER_2_Y) {
    message = WIN_MESSAGE;

    gameOver = true;
  }

  if (gameOver) {
    cv::putText(image, message, cv::Point(width/3,height/2), CV_FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(255,255,255));
    cv::imshow(WINDOW_NAME, image);
  }
}

void Pong::move(){
  int translation = std::min(std::abs(p1DestX - p1x), PLAYER_VELOCITY);
  if (p1DestX < p1x) translation = -translation;

  p1x += translation;  
}

void Pong::move(int x){
  p1DestX += x;

  if (p1DestX >= width - playerWidth/2) p1DestX = width - playerWidth/2;
  else if (p1DestX < playerWidth/2) p1DestX = playerWidth/2;
}

void Pong::moveTo(int x){
  p1DestX = x;

  if (p1DestX >= width - playerWidth/2) p1DestX = width - playerWidth/2;
  else if (p1DestX < playerWidth/2) p1DestX = playerWidth/2;  
}

void Pong::start(){
  gameOver = false;

  // Initialize players at center of screen
  p1x = width/2 - playerWidth/2;
  p2x = width/2 - playerWidth/2;
  p1DestX = p1x;
  p2DestX = p2x;

  // Initialize ball at center of screen
  ball.x = width/2;
  ball.y = height/2;
  ball.dir = -1;
  ball.mov = rand()%NUM_DIRECTIONS;

  numberHits = 0;
}

bool Pong::isGameOver(){
  return gameOver;
}

void Pong::update(){
  if (!gameOver){
    // Check for collisions
    checkWallCollision();
    checkPlayerCollision();

    // Move player
    move();

    // Move opponent
    moveOpponent();

    // Move ball
    moveBall();

    // Render scene
    draw();

    // Check for game end
    checkEndGame();
  }
}
/*
int main(){
  bool quit = false;

  Pong pong(640, 480);

  pong.start();
  while(!quit){
    pong.update();

    // Check for movement
    int k = cv::waitKey(10);
    switch(k){
      case 27:
        quit = true;
        break;
      case 'a':
        pong.move(-PLAYER_VELOCITY);
        break;
      case 'd':
        pong.move(PLAYER_VELOCITY);
        break;
    }
  }

  return 0;
}*/