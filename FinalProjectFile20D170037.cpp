#include <simplecpp>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <random>
using namespace simplecpp;

//movingObject.h
#ifndef _MOVINGOBJECT_INCLUDED_
#define _MOVINGOBJECT_INCLUDED_

#include <simplecpp>
#include <vector>
#include <composite.h>
#include <sprite.h>

using namespace simplecpp;

class MovingObject : public Sprite {
  vector<Sprite*> parts;
  double vx, vy;
  double ax, ay;
  bool paused;
  void initMO(double argvx, double argvy, double argax, double argay, bool argpaused=true) {
    vx=argvx; vy=argvy; ax=argax; ay=argay; paused=argpaused;
  }
 public:
 MovingObject(double argvx, double argvy, double argax, double argay, bool argpaused=true)
    : Sprite() {
    initMO(argvx, argvy, argax, argay, argpaused);
  }
 MovingObject(double speed, double angle_deg, double argax, double argay, bool argpaused, bool rtheta) : Sprite() {
   double angle_rad = angle_deg*PI/180.0;
   double argvx = speed*cos(angle_rad);
   double argvy = -speed*sin(angle_rad);
   initMO(argvx, argvy, argax, argay, argpaused);
  }
  void set_vx(double argvx) { vx = argvx; }
  void set_vy(double argvy) { vy = argvy; }
  void set_ax(double argax) { ax = argax; }
  void set_ay(double argay) { ay = argay; }
  double getXPos();
  double getYPos();
  void reset_all(double argx, double argy, double speed, double angle_deg, double argax, double argay, bool argpaused, bool rtheta);

  void pause() { paused = true; }
  void unpause() { paused = false; }
  bool isPaused() { return paused; }

  void addPart(Sprite* p) {
    parts.push_back(p);
  }
  void nextStep(double t);
  void getAttachedTo(MovingObject *m);
};

#endif

//MovingObject.cpp

void MovingObject::nextStep(double t) {
  if(paused) { return; }
  //cerr << "x=" << getXPos() << ",y=" << getYPos() << endl;
  //cerr << "vx=" << vx << ",vy=" << vy << endl;
  //cerr << "ax=" << ax << ",ay=" << ay << endl;

  for(size_t i=0; i<parts.size(); i++){
    parts[i]->move(vx*t, vy*t);
  }
  vx += ax*t;
  vy += ay*t;
} // End MovingObject::nextStep()

double MovingObject::getXPos() {
  return (parts.size() > 0) ? parts[0]->getX() : -1;//tertiary if else
}

double MovingObject::getYPos() {
  return (parts.size() > 0) ? parts[0]->getY() : -1;//tertiary if else
}

void MovingObject::reset_all(double argx, double argy, double speed, double angle_deg, double argax, double argay, bool argpaused, bool rtheta) {
  for(size_t i=0; i<parts.size(); i++){
    parts[i]->moveTo(argx, argy);
  }
  double angle_rad = angle_deg*PI/180.0;
  double argvx = speed*cos(angle_rad);
  double argvy = -speed*sin(angle_rad);
  vx = argvx; vy = argvy; ax = argax; ay = argay; paused = argpaused;
} // End MovingObject::reset_all()

void MovingObject::getAttachedTo(MovingObject *m) {//sticks coin to lasso, but not meant for multiple coins
  double xpos = m->getXPos();
  double ypos = m->getYPos();
  for(size_t i=0; i<parts.size(); i++){
    parts[i]->moveTo(xpos, ypos);
  }
  initMO(m->vx, m->vy, m->ax, m->ay, m->paused);
}

//coin.h
#ifndef __COIN_H__
#define __COIN_H__



class Coin : public MovingObject {
  double coin_start_x;
  double coin_start_y;
  double release_speed;
  double release_angle_deg;
  double coin_ax;
  double coin_ay;

  // Moving parts
  Circle coin_circle;

 public:
  int coin_point=1;
 Coin(double speed, double angle_deg, double argax, double argay, bool argpaused, bool rtheta) : MovingObject(speed, angle_deg, argax, argay, argpaused, rtheta) {
    release_speed = speed;
    release_angle_deg = angle_deg;
    coin_ax = argax;
    coin_ay = argay;
    initCoin();
  }//end Constructor

  void initCoin();
  void resetCoin();

  void change_to_bomb();
  void change_to_coin();
  void change_to_magnet();

}; // End class Coin

#endif

//lasso.h
#ifndef __LASSO_H__
#define __LASSO_H__

//#define WINDOW_X 1200
//#define WINDOW_Y 960
#define WINDOW_X 800
#define WINDOW_Y 600

#define STEP_TIME 0.05

#define PLAY_X_START 100
#define PLAY_Y_START 0
#define PLAY_X_WIDTH (WINDOW_X-PLAY_X_START)
#define PLAY_Y_HEIGHT (WINDOW_Y-100)

#define LASSO_X_OFFSET 100
#define LASSO_Y_HEIGHT 100
#define LASSO_BAND_LENGTH LASSO_X_OFFSET
#define LASSO_THICKNESS 5

#define COIN_GAP 1

#define RELEASE_ANGLE_STEP_DEG 5
#define MIN_RELEASE_ANGLE_DEG 0
#define MAX_RELEASE_ANGLE_DEG (360-RELEASE_ANGLE_STEP_DEG)
#define INIT_RELEASE_ANGLE_DEG 45

#define RELEASE_SPEED_STEP 20
#define MIN_RELEASE_SPEED 0
#define MAX_RELEASE_SPEED 200
#define INIT_RELEASE_SPEED 100

#define COIN_SPEED 120
#define COIN_ANGLE_DEG 90//could be changed for natural parabola

#define LASSO_G 30
#define COIN_G 30

#define LASSO_SIZE 10
#define LASSO_RADIUS 50
#define COIN_SIZE 5

#define NCOIN 10//max no. of coins

class Lasso : public MovingObject {
  double lasso_start_x;
  double lasso_start_y;
  double release_speed;
  double release_angle_deg;
  double lasso_ax;
  double lasso_ay;

  // Moving parts
  Circle lasso_circle;
  Circle lasso_loop;//defines radius when open

  // Non-moving parts
  Line lasso_line;
  Line lasso_band;

  // State info
  bool lasso_looped;
  Coin *the_coin;

  void initLasso();

 public:
   int num_coins;

 Lasso(double speed, double angle_deg, double argax, double argay, bool argpaused, bool rtheta) : MovingObject(speed, angle_deg, argax, argay, argpaused, rtheta) {
    release_speed = speed;
    release_angle_deg = angle_deg;
    lasso_ax = argax;
    lasso_ay = argay;
    initLasso();
  }

  void draw_lasso_band();
  void yank();
  void loopit();
  void addAngle(double angle_deg);
  void addSpeed(double speed);

  void nextStep(double t);
  void check_for_coin(Coin *coin);
  int getNumCoins() { return num_coins; }

}; // End class Lasso

#endif

//coin.h

void Coin::initCoin() {
  coin_start_x = (PLAY_X_START+WINDOW_X)/2;
  coin_start_y = PLAY_Y_HEIGHT;
  coin_circle.reset(coin_start_x, coin_start_y, COIN_SIZE);
  coin_circle.setColor(COLOR(255,215,0));//gold
  coin_circle.setFill(true);
  addPart(&coin_circle);
}

void Coin::resetCoin() {
  double coin_speed = COIN_SPEED;
  double coin_angle_deg = COIN_ANGLE_DEG;
  coin_ax = 0;
  coin_ay = COIN_G;
  bool paused = true, rtheta = true;
  reset_all(coin_start_x, coin_start_y, coin_speed, coin_angle_deg, coin_ax, coin_ay, paused, rtheta);
}

void Coin::change_to_bomb(){
    coin_circle.setColor(COLOR(0,0,0));//black
    coin_point=-3;
}//changes coin to bomb

void Coin::change_to_coin(){
    coin_circle.setColor(COLOR(255,215,0));//gold
    coin_point=1;
}//changes bomb to coin

void Coin::change_to_magnet(){
    coin_circle.setColor(COLOR(128,128,128));
    coin_point=0;
}//generates magnet coins

//lasso.cpp

void Lasso::draw_lasso_band() {
  double len = (release_speed/MAX_RELEASE_SPEED)*LASSO_BAND_LENGTH;
  double arad = release_angle_deg*PI/180.0;
  double xlen = len*cos(arad);
  double ylen = len*sin(arad);
  lasso_band.reset(lasso_start_x, lasso_start_y, (lasso_start_x-xlen), (lasso_start_y+ylen));
  lasso_band.setThickness(LASSO_THICKNESS);
} // End Lasso::draw_lasso_band()

void Lasso::initLasso() {
  lasso_start_x = (PLAY_X_START+LASSO_X_OFFSET);
  lasso_start_y = (PLAY_Y_HEIGHT-LASSO_Y_HEIGHT);
  lasso_circle.reset(lasso_start_x, lasso_start_y, LASSO_SIZE);
  lasso_circle.setColor(COLOR(255,0,0));//red
  lasso_circle.setFill(true);
  lasso_loop.reset(lasso_start_x, lasso_start_y, LASSO_SIZE/2);
  lasso_loop.setColor(COLOR(139,69,19));//brown
  lasso_loop.setFill(true);
  addPart(&lasso_circle);
  addPart(&lasso_loop);
  lasso_looped = false;
  the_coin = NULL;
  num_coins = 0;

  lasso_line.reset(lasso_start_x, lasso_start_y, lasso_start_x, lasso_start_y);
  lasso_line.setColor(COLOR(139,69,19));//brown

  lasso_band.setColor(COLOR(138,43,226));//BlueVoilet
  draw_lasso_band();

} // End Lasso::initLasso()

void Lasso::yank() {
  bool paused = true, rtheta = true;
  reset_all(lasso_start_x, lasso_start_y, release_speed, release_angle_deg, lasso_ax, lasso_ay, paused, rtheta);
  lasso_loop.reset(lasso_start_x, lasso_start_y, LASSO_SIZE/2);
  lasso_loop.setFill(true);
  lasso_looped = false;
  if(the_coin != NULL) {
    num_coins+=the_coin->coin_point;
    the_coin->resetCoin();
    the_coin = NULL;
  }
} // End Lasso::yank()

void Lasso::loopit() {
  if(lasso_looped) { return; } // Already looped
  lasso_loop.reset(getXPos(), getYPos(), LASSO_RADIUS);
  lasso_loop.setFill(false);
  lasso_looped = true;
} // End Lasso::loopit()

void Lasso::addAngle(double angle_deg) {
  release_angle_deg += angle_deg;
  if(release_angle_deg < MIN_RELEASE_ANGLE_DEG) { release_angle_deg = MIN_RELEASE_ANGLE_DEG; }
  if(release_angle_deg > MAX_RELEASE_ANGLE_DEG) { release_angle_deg = MAX_RELEASE_ANGLE_DEG; }
  bool paused = true, rtheta = true;
  reset_all(lasso_start_x, lasso_start_y, release_speed, release_angle_deg, lasso_ax, lasso_ay, paused, rtheta);
} // End Lasso::addAngle()

void Lasso::addSpeed(double speed) {
  release_speed += speed;
  if(release_speed < MIN_RELEASE_SPEED) { release_speed = MIN_RELEASE_SPEED; }
  if(release_speed > MAX_RELEASE_SPEED) { release_speed = MAX_RELEASE_SPEED; }
  bool paused = true, rtheta = true;
  reset_all(lasso_start_x, lasso_start_y, release_speed, release_angle_deg, lasso_ax, lasso_ay, paused, rtheta);
} // End Lasso::addSpeed()

void Lasso::nextStep(double stepTime) {
  draw_lasso_band();
  MovingObject::nextStep(stepTime);
  if(getYPos() > PLAY_Y_HEIGHT) { yank(); }
  lasso_line.reset(lasso_start_x, lasso_start_y, getXPos(), getYPos());
} // End Lasso::nextStep()

void Lasso::check_for_coin(Coin *coinPtr) {
  double lasso_x = getXPos();
  double lasso_y = getYPos();
  double coin_x = coinPtr->getXPos();
  double coin_y = coinPtr->getYPos();
  double xdiff = (lasso_x - coin_x);
  double ydiff = (lasso_y - coin_y);
  double distance = sqrt((xdiff*xdiff)+(ydiff*ydiff));
  if(distance <= LASSO_RADIUS) {
    the_coin = coinPtr;
    the_coin->getAttachedTo(this);
  }
} // End Lasso::check_for_coin()

#define SPACE 20 // space between lines for instructions;
#define MARGIN 40 // margin from the edges in canvas;

void helppage(){

    Text heading( WINDOW_X/2, MARGIN, "THE LASSO GAME");
    Text description{WINDOW_X/2, MARGIN+2*SPACE, "This is a game about catching coins with a Lasso. The controls for the lasso are given below." };
    Text description_continued{ WINDOW_X/2, MARGIN+3*SPACE, "The number and placement of the coins, as well as Bombs and collectibles will change as the game progresses."};
    Text subheading{WINDOW_X/2, MARGIN+5*SPACE, "LASSO CONTROLS"};
    Text start{WINDOW_X/2, MARGIN+7*SPACE, "Press 'T' to throw lasso"};
    Text loop{WINDOW_X/2, MARGIN+8*SPACE, "Press 'L' to catch coin when in range"};
    Text yank{WINDOW_X/2, MARGIN+9*SPACE, "Press 'Y' to yank the lasso back"};
    Text speed{WINDOW_X/2, MARGIN+10*SPACE, "Use '=' and '-' to control speed of lasso"};
    Text angle{WINDOW_X/2, MARGIN+11*SPACE, "Use '[' and ']' to change angle of ejection for lasso"};
    Text quit{WINDOW_X/2, MARGIN+12*SPACE, "Press 'q' to quit"};
    Text confirmation{WINDOW_X-3*MARGIN, WINDOW_Y-MARGIN, "Click anywhere to start"};

    getClick();//this adds the function of waiting for the user to click their mouse to start/countinue the game

}//help document at start

void magnet(double &ax){
    ax=COIN_G;
}// magnet increases acceleration towards cannon, not lasso

void unmagnet(double &ax){
    ax=0;
}//default magnet function

main_program {

  cout<<"Start game? (y/n)"<<endl;
  char confirm;
  cin>>confirm;
  if(confirm=='n')exit(0);
  cout<<"Aim of the game is to catch a particular number of coins within the given time."<<endl;
  cout<<"Choose a level of difficulty from 1(easy) to 5(difficult). \nThe game starts from this level and continues to next level as you gain necessary amount of coins. \nPress q at any point to exit"<<endl;
  int lvl = 1;//level no.
  cin>>lvl;

  initCanvas("Lasso", WINDOW_X, WINDOW_Y);
  helppage();

  int stepCount = 0;
  float stepTime = STEP_TIME;
  float runTime = 70; // sec; -ve means infinite
  float currTime = 0;
  int lf = 3;//no of lives, may change according to level
  int maxscore=4;//max score to achieve before moving to next level, may change with level

  // Draw lasso at start position
  double release_speed = INIT_RELEASE_SPEED; // m/s
  double release_angle_deg = INIT_RELEASE_ANGLE_DEG; // degrees
  double lasso_ax = 0;
  double lasso_ay = LASSO_G;
  bool paused = true;
  bool rtheta = true;
  Lasso lasso(release_speed, release_angle_deg, lasso_ax, lasso_ay, paused, rtheta);

  Line b1(0, PLAY_Y_HEIGHT, WINDOW_X, PLAY_Y_HEIGHT);
  b1.setColor(COLOR(0,0,255));//blue
  Line b2(PLAY_X_START, 0, PLAY_X_START, WINDOW_Y);
  b2.setColor(COLOR(0,0,255));//blue

  string msg("Cmd: _");
  Text charPressed(PLAY_X_START+50, PLAY_Y_HEIGHT+20, msg);
  char coinScoreStr[256];
  sprintf(coinScoreStr, "Coins: %d", lasso.getNumCoins());
  Text coinScore(PLAY_X_START+50, PLAY_Y_HEIGHT+50, coinScoreStr);

  if(lvl==1){
    string level("Level: _");//displaying level no.
  Text currentLevel(PLAY_X_START+50, PLAY_Y_HEIGHT+80, level);
  level[level.length()-1] = '0'+char(lvl);
  currentLevel.setMessage(level);

  string lives("Lives: _");//displaying no of lives left
  Text currentLives(WINDOW_X-3*MARGIN, MARGIN, lives);
  lives[lives.length()-1] = '0'+char(lf);
  currentLives.setMessage(lives);

  paused = true; rtheta = true;
  double coin_speed = COIN_SPEED;
  double coin_angle_deg = COIN_ANGLE_DEG;
  double coin_ax = 0;
  double coin_ay = COIN_G;

  while(lf>0){
  currTime=0;//reset CurrentTime
  Coin coin(coin_speed, coin_angle_deg, coin_ax, coin_ay, paused, rtheta);

  // After every COIN_GAP sec, make the coin jump
  double last_coin_jump_end = 0;
  // When t is pressed, throw lasso
  // If lasso within range, make coin stick
  // When y is pressed, yank lasso
  // When l is pressed, loop lasso
  // When q is pressed, quit

  for(;;) {
    if(lasso.getNumCoins()==maxscore)break;
    if(((runTime > 0) && (currTime > runTime) && (lasso.getNumCoins()<maxscore)) || lasso.getNumCoins()<0) {
    lf--;
    if(lasso.getNumCoins()<0)lasso.num_coins=0;
    lives[lives.length()-1] = '0'+char(lf);
    currentLives.setMessage(lives);
    Text lfLost(WINDOW_X/2, WINDOW_Y/2, "YOU HAVE LOST A LIFE. Click to continue.");
    getClick();
    if(lf==0){cout<<"GAME OVER"<<endl; exit(0);}
    break;
    }

    char time[256];
    sprintf(time, "remaing time: %.2f", runTime-currTime);
    Text Time(450,50, time);

    XEvent e;
    bool pendingEv = checkEvent(e);
    if(pendingEv) {
      char c = charFromEvent(e);
      msg[msg.length()-1] = c;
      charPressed.setMessage(msg);
      switch(c) {
      case 't':
	lasso.unpause();
	break;
      case 'y':
	lasso.yank();
	break;
      case 'l':
	lasso.loopit();
	lasso.check_for_coin(&coin);
	wait(STEP_TIME*5);
	break;
      case '[':
	if(lasso.isPaused()) { lasso.addAngle(-RELEASE_ANGLE_STEP_DEG);	}
	break;
      case ']':
	if(lasso.isPaused()) { lasso.addAngle(+RELEASE_ANGLE_STEP_DEG); }
	break;
      case '-':
	if(lasso.isPaused()) { lasso.addSpeed(-RELEASE_SPEED_STEP); }
	break;
      case '=':
	if(lasso.isPaused()) { lasso.addSpeed(+RELEASE_SPEED_STEP); }
	break;
      case 'q':
	exit(0);
      default:
	break;
      }
    }

    lasso.nextStep(stepTime);

    coin.nextStep(stepTime);

    if(coin.isPaused()) {
      if((currTime-last_coin_jump_end) >= COIN_GAP) {
	coin.unpause();
      }
    }

    if(coin.getYPos() > PLAY_Y_HEIGHT) {
      coin.resetCoin();
      coin.reset_all(PLAY_X_START*4 + WINDOW_X/2*(rand()%100)/100.0, PLAY_Y_HEIGHT, coin_speed, coin_angle_deg, coin_ax, coin_ay, paused, rtheta);
      last_coin_jump_end = currTime;
    }

    sprintf(coinScoreStr, "Coins: %d", lasso.getNumCoins());
    coinScore.setMessage(coinScoreStr);

    stepCount++;
    currTime += stepTime;
    wait(stepTime);
  } // End for(;;)

  wait(3);
  }//end while for life
}//end lvl 1

  else if(lvl==2){
  lf+=3;
  string level("Level: _");//displaying level no.
  Text currentLevel(PLAY_X_START+50, PLAY_Y_HEIGHT+80, level);
  level[level.length()-1] = '0'+char(lvl);
  currentLevel.setMessage(level);

  string lives("Lives: _");//displaying no of lives left
  Text currentLives(WINDOW_X-3*MARGIN, MARGIN, lives);
  lives[lives.length()-1] = '0'+char(lf);
  currentLives.setMessage(lives);

  paused = true; rtheta = true;
  double coin_speed = COIN_SPEED;
  double coin_angle_deg = COIN_ANGLE_DEG;
  double coin_ax = 0;
  double coin_ay = COIN_G;

  while(lf>0){
  currTime=0;//reset CurrentTime
  Coin coin1(coin_speed, coin_angle_deg, coin_ax, coin_ay, paused, rtheta);
  coin1.reset_all(PLAY_X_START*4 + WINDOW_X/2*(rand()%100)/100.0, PLAY_Y_HEIGHT, coin_speed, coin_angle_deg, coin_ax, coin_ay, paused, rtheta);
  Coin coin2(coin_speed, coin_angle_deg, coin_ax, coin_ay, paused, rtheta);
  coin2.reset_all(PLAY_X_START*4 + WINDOW_X/2*(rand()%100)/100.0, PLAY_Y_HEIGHT, coin_speed, coin_angle_deg, coin_ax, coin_ay, paused, rtheta);
  Coin coin3(coin_speed, coin_angle_deg, coin_ax, coin_ay, paused, rtheta);
  coin3.reset_all(PLAY_X_START*4 + WINDOW_X/2*(rand()%100)/100.0, PLAY_Y_HEIGHT, coin_speed, coin_angle_deg, coin_ax, coin_ay, paused, rtheta);

  // After every COIN_GAP sec, make the coin jump
  double last_coin_jump_end = 0;
  // When t is pressed, throw lasso
  // If lasso within range, make coin stick
  // When y is pressed, yank lasso
  // When l is pressed, loop lasso
  // When q is pressed, quit

  for(;;) {
    if(lasso.getNumCoins()==maxscore+1)break;
    if(((runTime > 0) && (currTime > runTime) && (lasso.getNumCoins()<maxscore+1)) || lasso.getNumCoins()<0) {
    lf--;
    if(lasso.getNumCoins()<0)lasso.num_coins=0;
    lives[lives.length()-1] = '0'+char(lf);
    currentLives.setMessage(lives);
    Text lfLost(WINDOW_X/2, WINDOW_Y/2, "YOU HAVE LOST A LIFE. Click to continue.");
    getClick();
    if(lf==0){cout<<"GAME OVER"<<endl; exit(0);}
    break;
    }

    char time[256];
    sprintf(time, "remaing time: %.2f", runTime-currTime);
    Text Time(450,50, time);

    XEvent e;
    bool pendingEv = checkEvent(e);
    if(pendingEv) {
      char c = charFromEvent(e);
      msg[msg.length()-1] = c;
      charPressed.setMessage(msg);
      switch(c) {
      case 't':
	lasso.unpause();
	break;
      case 'y':
	lasso.yank();
	break;
      case 'l':
	lasso.loopit();
	lasso.check_for_coin(&coin1);
	lasso.check_for_coin(&coin2);
	lasso.check_for_coin(&coin3);
	wait(STEP_TIME*5);
	break;
      case '[':
	if(lasso.isPaused()) { lasso.addAngle(-RELEASE_ANGLE_STEP_DEG);	}
	break;
      case ']':
	if(lasso.isPaused()) { lasso.addAngle(+RELEASE_ANGLE_STEP_DEG); }
	break;
      case '-':
	if(lasso.isPaused()) { lasso.addSpeed(-RELEASE_SPEED_STEP); }
	break;
      case '=':
	if(lasso.isPaused()) { lasso.addSpeed(+RELEASE_SPEED_STEP); }
	break;
      case 'q':
	exit(0);
      default:
	break;
      }
    }

    lasso.nextStep(stepTime);

    coin1.nextStep(stepTime);
    coin2.nextStep(stepTime);
    coin3.nextStep(stepTime);

    if(coin1.isPaused()) {
      if((currTime-last_coin_jump_end) >= COIN_GAP) {
	coin1.unpause();
      }
    }

    if(coin1.getYPos() > PLAY_Y_HEIGHT) {
      coin1.resetCoin();
      coin1.reset_all(PLAY_X_START*4 + WINDOW_X/2*(rand()%100)/100.0, PLAY_Y_HEIGHT, coin_speed, coin_angle_deg, coin_ax, coin_ay, paused, rtheta);
      last_coin_jump_end = currTime;
    }

     if(coin2.isPaused()) {
      if((currTime-last_coin_jump_end) >= COIN_GAP) {
	coin2.unpause();
      }
    }

    if(coin2.getYPos() > PLAY_Y_HEIGHT) {
      coin2.resetCoin();
      coin2.reset_all(PLAY_X_START*4 + WINDOW_X/2*(rand()%100)/100.0, PLAY_Y_HEIGHT, coin_speed, coin_angle_deg, coin_ax, coin_ay, paused, rtheta);
      last_coin_jump_end = currTime;
    }

     if(coin3.isPaused()) {
      if((currTime-last_coin_jump_end) >= COIN_GAP) {
	coin3.unpause();
      }
    }

    if(coin3.getYPos() > PLAY_Y_HEIGHT) {
      coin3.resetCoin();
      coin3.reset_all(PLAY_X_START*4 + WINDOW_X/2*(rand()%100)/100.0, PLAY_Y_HEIGHT, coin_speed, coin_angle_deg, coin_ax, coin_ay, paused, rtheta);
      last_coin_jump_end = currTime;
    }

    sprintf(coinScoreStr, "Coins: %d", lasso.getNumCoins());
    coinScore.setMessage(coinScoreStr);

    stepCount++;
    currTime += stepTime;
    wait(stepTime);
  } // End for(;;)

  wait(3);
  }//end life loop
}//end lvl2

  else if(lvl==3){
  lf+=3;
  string level("Level: _");//displaying level no.
  Text currentLevel(PLAY_X_START+50, PLAY_Y_HEIGHT+80, level);
  level[level.length()-1] = '0'+char(lvl);
  currentLevel.setMessage(level);

  string lives("Lives: _");//displaying no of lives left
  Text currentLives(WINDOW_X-3*MARGIN, MARGIN, lives);
  lives[lives.length()-1] = '0'+char(lf);
  currentLives.setMessage(lives);

  paused = true; rtheta = true;
  double coin_speed = COIN_SPEED;
  double coin_angle_deg = COIN_ANGLE_DEG;
  double coin_ax = 0;
  double coin_ay = COIN_G;

  while(lf>0){
  currTime=0;//reset CurrentTime
  Coin coin1(coin_speed, coin_angle_deg, coin_ax, coin_ay, paused, rtheta);
  coin1.reset_all(PLAY_X_START*4 + WINDOW_X/2*(rand()%100)/100.0, PLAY_Y_HEIGHT, coin_speed, coin_angle_deg, coin_ax, coin_ay, paused, rtheta);
  Coin coin2(coin_speed, coin_angle_deg, coin_ax, coin_ay, paused, rtheta);
  coin2.reset_all(PLAY_X_START*4 + WINDOW_X/2*(rand()%100)/100.0, PLAY_Y_HEIGHT, coin_speed, coin_angle_deg, coin_ax, coin_ay, paused, rtheta);
  Coin coin3(coin_speed, coin_angle_deg, coin_ax, coin_ay, paused, rtheta);
  coin3.reset_all(PLAY_X_START*4 + WINDOW_X/2*(rand()%100)/100.0, PLAY_Y_HEIGHT, coin_speed, coin_angle_deg, coin_ax, coin_ay, paused, rtheta);

  // After every COIN_GAP sec, make the coin jump
  double last_coin_jump_end = 0;
  // When t is pressed, throw lasso
  // If lasso within range, make coin stick
  // When y is pressed, yank lasso
  // When l is pressed, loop lasso
  // When q is pressed, quit

  for(;;) {
    if(lasso.getNumCoins()==maxscore+1)break;
    if(((runTime > 0) && (currTime > runTime) && (lasso.getNumCoins()<maxscore+1)) || lasso.getNumCoins()<0) {
    lf--;
    if(lasso.getNumCoins()<0)lasso.num_coins=0;
    lives[lives.length()-1] = '0'+char(lf);
    currentLives.setMessage(lives);
    Text lfLost(WINDOW_X/2, WINDOW_Y/2, "YOU HAVE LOST A LIFE. Click to continue.");
    getClick();
    if(lf==0){cout<<"GAME OVER"<<endl; exit(0);}
    break;
    }

    char time[256];
    sprintf(time, "remaing time: %.2f", runTime-currTime);
    Text Time(450,50, time);

    XEvent e;
    bool pendingEv = checkEvent(e);
    if(pendingEv) {
      char c = charFromEvent(e);
      msg[msg.length()-1] = c;
      charPressed.setMessage(msg);
      switch(c) {
      case 't':
	lasso.unpause();
	break;
      case 'y':
	lasso.yank();
	break;
      case 'l':
	lasso.loopit();
	lasso.check_for_coin(&coin1);
	lasso.check_for_coin(&coin2);
	lasso.check_for_coin(&coin3);
	wait(STEP_TIME*5);
	break;
      case '[':
	if(lasso.isPaused()) { lasso.addAngle(-RELEASE_ANGLE_STEP_DEG);	}
	break;
      case ']':
	if(lasso.isPaused()) { lasso.addAngle(+RELEASE_ANGLE_STEP_DEG); }
	break;
      case '-':
	if(lasso.isPaused()) { lasso.addSpeed(-RELEASE_SPEED_STEP); }
	break;
      case '=':
	if(lasso.isPaused()) { lasso.addSpeed(+RELEASE_SPEED_STEP); }
	break;
      case 'q':
	exit(0);
      default:
	break;
      }
    }

    lasso.nextStep(stepTime);

    coin1.nextStep(stepTime);
    coin2.nextStep(stepTime);
    coin3.nextStep(stepTime);

    if(coin1.isPaused()) {
      if((currTime-last_coin_jump_end) >= COIN_GAP) {
	coin1.unpause();
      }
    }

    if(coin1.getYPos() > PLAY_Y_HEIGHT) {
      coin1.resetCoin();
      coin1.reset_all(PLAY_X_START*4 + WINDOW_X/2*(rand()%100)/100.0, PLAY_Y_HEIGHT, coin_speed, coin_angle_deg, coin_ax, coin_ay, paused, rtheta);
      if(rand()%10 >4){
        coin1.change_to_bomb();
      }
      else{
        coin1.change_to_coin();
      }
      last_coin_jump_end = currTime;
    }

     if(coin2.isPaused()) {
      if((currTime-last_coin_jump_end) >= COIN_GAP) {
	coin2.unpause();
      }
    }

    if(coin2.getYPos() > PLAY_Y_HEIGHT) {
      coin2.resetCoin();
      coin2.reset_all(PLAY_X_START*4 + WINDOW_X/2*(rand()%100)/100.0, PLAY_Y_HEIGHT, coin_speed, coin_angle_deg, coin_ax, coin_ay, paused, rtheta);
      if(rand()%10 >4){
        coin2.change_to_bomb();
      }
      else{
        coin2.change_to_coin();
      }
      last_coin_jump_end = currTime;
    }

     if(coin3.isPaused()) {
      if((currTime-last_coin_jump_end) >= COIN_GAP) {
	coin3.unpause();
      }
    }

    if(coin3.getYPos() > PLAY_Y_HEIGHT) {
      coin3.resetCoin();
      coin3.reset_all(PLAY_X_START*4 + WINDOW_X/2*(rand()%100)/100.0, PLAY_Y_HEIGHT, coin_speed, coin_angle_deg, coin_ax, coin_ay, paused, rtheta);
      if(rand()%10 >4){
        coin3.change_to_bomb();
      }
      else{
        coin3.change_to_coin();
      }
      last_coin_jump_end = currTime;
    }

    sprintf(coinScoreStr, "Coins: %d", lasso.getNumCoins());
    coinScore.setMessage(coinScoreStr);

    stepCount++;
    currTime += stepTime;
    wait(stepTime);
  } // End for(;;)
  } // end life loop
}//end lvl 3

  else if(lvl==4){
  lf+=2;
  int magno=-1;//-ve means neutral
  string level("Level: _");//displaying level no.
  Text currentLevel(PLAY_X_START+50, PLAY_Y_HEIGHT+80, level);
  level[level.length()-1] = '0'+char(lvl);
  currentLevel.setMessage(level);

  string lives("Lives: _");//displaying no of lives left
  Text currentLives(WINDOW_X-3*MARGIN, MARGIN, lives);
  lives[lives.length()-1] = '0'+char(lf);
  currentLives.setMessage(lives);

  paused = true; rtheta = true;
  double coin_speed = COIN_SPEED;
  double coin_angle_deg = COIN_ANGLE_DEG;
  double coin_ax = 0;
  double coin_ay = COIN_G;

  while(lf>0){
  currTime=0;//reset CurrentTime
  Coin coin1(coin_speed, coin_angle_deg, coin_ax, coin_ay, paused, rtheta);
  coin1.reset_all(PLAY_X_START*4 + WINDOW_X/2*(rand()%100)/100.0, PLAY_Y_HEIGHT, coin_speed, coin_angle_deg, coin_ax, coin_ay, paused, rtheta);
  Coin coin2(coin_speed, coin_angle_deg, coin_ax, coin_ay, paused, rtheta);
  coin2.reset_all(PLAY_X_START*4 + WINDOW_X/2*(rand()%100)/100.0, PLAY_Y_HEIGHT, coin_speed, coin_angle_deg, coin_ax, coin_ay, paused, rtheta);
  Coin coin3(coin_speed, coin_angle_deg, coin_ax, coin_ay, paused, rtheta);
  coin3.reset_all(PLAY_X_START*4 + WINDOW_X/2*(rand()%100)/100.0, PLAY_Y_HEIGHT, coin_speed, coin_angle_deg, coin_ax, coin_ay, paused, rtheta);

  // After every COIN_GAP sec, make the coin jump
  double last_coin_jump_end = 0;
  // When t is pressed, throw lasso
  // If lasso within range, make coin stick
  // When y is pressed, yank lasso
  // When l is pressed, loop lasso
  // When q is pressed, quit

  for(;;) {
    if(lasso.getNumCoins()==maxscore+1)break;
    if(((runTime > 0) && (currTime > runTime) && (lasso.getNumCoins()<maxscore+1)) || lasso.getNumCoins()<0) {
    lf--;
    if(lasso.getNumCoins()<0)lasso.num_coins=0;
    lives[lives.length()-1] = '0'+char(lf);
    currentLives.setMessage(lives);
    Text lfLost(WINDOW_X/2, WINDOW_Y/2, "YOU HAVE LOST A LIFE. Click to continue.");
    getClick();
    if(lf==0){cout<<"GAME OVER"<<endl; exit(0);}
    break;
    }

    char time[256];
    sprintf(time, "remaing time: %.2f", runTime-currTime);
    Text Time(450,50, time);

    XEvent e;
    bool pendingEv = checkEvent(e);
    if(pendingEv) {
      char c = charFromEvent(e);
      msg[msg.length()-1] = c;
      charPressed.setMessage(msg);
      switch(c) {
      case 't':
	lasso.unpause();
	break;
      case 'y':
	lasso.yank();
	break;
      case 'l':
	lasso.loopit();
	lasso.check_for_coin(&coin1);
        {
    double lasso_x = lasso.getXPos();
    double lasso_y = lasso.getYPos();
    double coin_x = coin1.getXPos();
    double coin_y = coin1.getYPos();
    double xdiff = (lasso_x - coin_x);
    double ydiff = (lasso_y - coin_y);
    double distance = sqrt((xdiff*xdiff)+(ydiff*ydiff));
    if(distance <= LASSO_RADIUS || coin1.coin_point==0) {
        magnet(coin_ax);
        magno=100;
        }
    }
	lasso.check_for_coin(&coin2);
	 {
    double lasso_x = lasso.getXPos();
    double lasso_y = lasso.getYPos();
    double coin_x = coin2.getXPos();
    double coin_y = coin2.getYPos();
    double xdiff = (lasso_x - coin_x);
    double ydiff = (lasso_y - coin_y);
    double distance = sqrt((xdiff*xdiff)+(ydiff*ydiff));
    if(distance <= LASSO_RADIUS || coin2.coin_point==0) {
        magnet(coin_ax);
        magno=100;
        }
    }
	lasso.check_for_coin(&coin3);
	 {
    double lasso_x = lasso.getXPos();
    double lasso_y = lasso.getYPos();
    double coin_x = coin3.getXPos();
    double coin_y = coin3.getYPos();
    double xdiff = (lasso_x - coin_x);
    double ydiff = (lasso_y - coin_y);
    double distance = sqrt((xdiff*xdiff)+(ydiff*ydiff));
    if(distance <= LASSO_RADIUS || coin3.coin_point==0) {
        magnet(coin_ax);
        magno=100;
        }
    }
	wait(STEP_TIME*5);
	break;
      case '[':
	if(lasso.isPaused()) { lasso.addAngle(-RELEASE_ANGLE_STEP_DEG);	}
	break;
      case ']':
	if(lasso.isPaused()) { lasso.addAngle(+RELEASE_ANGLE_STEP_DEG); }
	break;
      case '-':
	if(lasso.isPaused()) { lasso.addSpeed(-RELEASE_SPEED_STEP); }
	break;
      case '=':
	if(lasso.isPaused()) { lasso.addSpeed(+RELEASE_SPEED_STEP); }
	break;
      case 'q':
	exit(0);
      default:
	break;
      }
    }

    lasso.nextStep(stepTime);

    coin1.nextStep(stepTime);
    coin2.nextStep(stepTime);
    coin3.nextStep(stepTime);

    if(coin1.isPaused()) {
      if((currTime-last_coin_jump_end) >= COIN_GAP) {
	coin1.unpause();
      }
    }

    if(coin1.getYPos() > PLAY_Y_HEIGHT) {
      coin1.resetCoin();
      coin1.reset_all(PLAY_X_START*4 + WINDOW_X/2*(rand()%100)/100.0, PLAY_Y_HEIGHT, coin_speed, coin_angle_deg, coin_ax, coin_ay, paused, rtheta);
      if(rand()%10 >4){
        coin1.change_to_bomb();
      }
      else{
        coin1.change_to_coin();
      }
      if(rand()%100 <5){
        coin1.change_to_magnet();
      }
      last_coin_jump_end = currTime;
    }

     if(coin2.isPaused()) {
      if((currTime-last_coin_jump_end) >= COIN_GAP) {
	coin2.unpause();
      }
    }

    if(coin2.getYPos() > PLAY_Y_HEIGHT) {
      coin2.resetCoin();
      coin2.reset_all(PLAY_X_START*4 + WINDOW_X/2*(rand()%100)/100.0, PLAY_Y_HEIGHT, coin_speed, coin_angle_deg, coin_ax, coin_ay, paused, rtheta);
      if(rand()%10 >4){
        coin2.change_to_bomb();
      }
      else{
        coin2.change_to_coin();
      }
      if(rand()%100 <5){
        coin2.change_to_magnet();
      }
      last_coin_jump_end = currTime;
    }

     if(coin3.isPaused()) {
      if((currTime-last_coin_jump_end) >= COIN_GAP) {
	coin3.unpause();
      }
    }

    if(coin3.getYPos() > PLAY_Y_HEIGHT) {
      coin3.resetCoin();
      coin3.reset_all(PLAY_X_START*4 + WINDOW_X/2*(rand()%100)/100.0, PLAY_Y_HEIGHT, coin_speed, coin_angle_deg, coin_ax, coin_ay, paused, rtheta);
      if(rand()%10 >4){
        coin3.change_to_bomb();
      }
      else{
        coin3.change_to_coin();
      }
      if(rand()%100 <5){
        coin3.change_to_magnet();
      }
      last_coin_jump_end = currTime;
    }

    sprintf(coinScoreStr, "Coins: %d", lasso.getNumCoins());
    coinScore.setMessage(coinScoreStr);

    stepCount++;
    if(magno>=0)magno--;
    if(magno<0)unmagnet(coin_ax);
    currTime += stepTime;
    wait(stepTime);
  } // End for(;;)
  } // end life loop
}// end lvl 4

  else if(lvl==5){
  lf+=2;
  int magno=-1;//-ve means neutral
  string level("Level: _");//displaying level no.
  Text currentLevel(PLAY_X_START+50, PLAY_Y_HEIGHT+80, level);
  level[level.length()-1] = '0'+char(lvl);
  currentLevel.setMessage(level);

  string lives("Lives: _");//displaying no of lives left
  Text currentLives(WINDOW_X-3*MARGIN, MARGIN, lives);
  lives[lives.length()-1] = '0'+char(lf);
  currentLives.setMessage(lives);

  paused = true; rtheta = true;
  double coin_speed = COIN_SPEED;
  double coin_angle_deg = COIN_ANGLE_DEG;
  double coin_ax = 0;
  double coin_ay = COIN_G;

  while(lf>0){
  currTime=0;//reset CurrentTime
  Coin coin1(coin_speed, coin_angle_deg, coin_ax, coin_ay, paused, rtheta);
  coin1.reset_all(PLAY_X_START*4 + WINDOW_X/2*(rand()%100)/100.0, PLAY_Y_HEIGHT, coin_speed, 2*coin_angle_deg*(rand()%100)/100.0, coin_ax, coin_ay, paused, rtheta);
  Coin coin2(coin_speed, coin_angle_deg, coin_ax, coin_ay, paused, rtheta);
  coin2.reset_all(PLAY_X_START*4 + WINDOW_X/2*(rand()%100)/100.0, PLAY_Y_HEIGHT, coin_speed, 2*coin_angle_deg*(rand()%100)/100.0, coin_ax, coin_ay, paused, rtheta);
  Coin coin3(coin_speed, coin_angle_deg, coin_ax, coin_ay, paused, rtheta);
  coin3.reset_all(PLAY_X_START*4 + WINDOW_X/2*(rand()%100)/100.0, PLAY_Y_HEIGHT, coin_speed, 2*coin_angle_deg*(rand()%100)/100.0, coin_ax, coin_ay, paused, rtheta);

  // After every COIN_GAP sec, make the coin jump
  double last_coin_jump_end = 0;
  // When t is pressed, throw lasso
  // If lasso within range, make coin stick
  // When y is pressed, yank lasso
  // When l is pressed, loop lasso
  // When q is pressed, quit

  for(;;) {
    if(lasso.getNumCoins()==maxscore+2)break;
    if(((runTime > 0) && (currTime > runTime) && (lasso.getNumCoins()<maxscore+2)) || lasso.getNumCoins()<0) {
    lf--;
    if(lasso.getNumCoins()<0)lasso.num_coins=0;
    lives[lives.length()-1] = '0'+char(lf);
    currentLives.setMessage(lives);
    Text lfLost(WINDOW_X/2, WINDOW_Y/2, "YOU HAVE LOST A LIFE. Click to continue.");
    getClick();
    if(lf==0){cout<<"GAME OVER"<<endl; exit(0);}
    break;
    }

    char time[256];
    sprintf(time, "remaing time: %.2f", runTime-currTime);
    Text Time(450,50, time);

    XEvent e;
    bool pendingEv = checkEvent(e);
    if(pendingEv) {
      char c = charFromEvent(e);
      msg[msg.length()-1] = c;
      charPressed.setMessage(msg);
      switch(c) {
      case 't':
	lasso.unpause();
	break;
      case 'y':
	lasso.yank();
	break;
      case 'l':
	lasso.loopit();
	lasso.check_for_coin(&coin1);
        {
    double lasso_x = lasso.getXPos();
    double lasso_y = lasso.getYPos();
    double coin_x = coin1.getXPos();
    double coin_y = coin1.getYPos();
    double xdiff = (lasso_x - coin_x);
    double ydiff = (lasso_y - coin_y);
    double distance = sqrt((xdiff*xdiff)+(ydiff*ydiff));
    if(distance <= LASSO_RADIUS || coin1.coin_point==0) {
        magnet(coin_ax);
        magno=100;
        }
    }
	lasso.check_for_coin(&coin2);
	 {
    double lasso_x = lasso.getXPos();
    double lasso_y = lasso.getYPos();
    double coin_x = coin2.getXPos();
    double coin_y = coin2.getYPos();
    double xdiff = (lasso_x - coin_x);
    double ydiff = (lasso_y - coin_y);
    double distance = sqrt((xdiff*xdiff)+(ydiff*ydiff));
    if(distance <= LASSO_RADIUS || coin2.coin_point==0) {
        magnet(coin_ax);
        magno=100;
        }
    }
	lasso.check_for_coin(&coin3);
	 {
    double lasso_x = lasso.getXPos();
    double lasso_y = lasso.getYPos();
    double coin_x = coin3.getXPos();
    double coin_y = coin3.getYPos();
    double xdiff = (lasso_x - coin_x);
    double ydiff = (lasso_y - coin_y);
    double distance = sqrt((xdiff*xdiff)+(ydiff*ydiff));
    if(distance <= LASSO_RADIUS || coin3.coin_point==0) {
        magnet(coin_ax);
        magno=100;
        }
    }
	wait(STEP_TIME*5);
	break;
      case '[':
	if(lasso.isPaused()) { lasso.addAngle(-RELEASE_ANGLE_STEP_DEG);	}
	break;
      case ']':
	if(lasso.isPaused()) { lasso.addAngle(+RELEASE_ANGLE_STEP_DEG); }
	break;
      case '-':
	if(lasso.isPaused()) { lasso.addSpeed(-RELEASE_SPEED_STEP); }
	break;
      case '=':
	if(lasso.isPaused()) { lasso.addSpeed(+RELEASE_SPEED_STEP); }
	break;
      case 'q':
	exit(0);
      default:
	break;
      }
    }

    lasso.nextStep(stepTime);

    coin1.nextStep(stepTime);
    coin2.nextStep(stepTime);
    coin3.nextStep(stepTime);

    if(coin1.isPaused()) {
      if((currTime-last_coin_jump_end) >= COIN_GAP) {
	coin1.unpause();
      }
    }

    if(coin1.getYPos() > PLAY_Y_HEIGHT) {
      coin1.resetCoin();
      coin1.reset_all(PLAY_X_START*4 + WINDOW_X/2*(rand()%100)/100.0, PLAY_Y_HEIGHT, coin_speed, 2*coin_angle_deg*(rand()%100)/100.0, coin_ax, coin_ay, paused, rtheta);
      if(rand()%10 >4){
        coin1.change_to_bomb();
      }
      else{
        coin1.change_to_coin();
      }
      if(rand()%100 <5){
        coin1.change_to_magnet();
      }
      last_coin_jump_end = currTime;
    }

     if(coin2.isPaused()) {
      if((currTime-last_coin_jump_end) >= COIN_GAP) {
	coin2.unpause();
      }
    }

    if(coin2.getYPos() > PLAY_Y_HEIGHT) {
      coin2.resetCoin();
      coin2.reset_all(PLAY_X_START*4 + WINDOW_X/2*(rand()%100)/100.0, PLAY_Y_HEIGHT, coin_speed, 2*coin_angle_deg*(rand()%100)/100.0, coin_ax, coin_ay, paused, rtheta);
      if(rand()%10 >4){
        coin2.change_to_bomb();
      }
      else{
        coin2.change_to_coin();
      }
      if(rand()%100 <5){
        coin2.change_to_magnet();
      }
      last_coin_jump_end = currTime;
    }

     if(coin3.isPaused()) {
      if((currTime-last_coin_jump_end) >= COIN_GAP) {
	coin3.unpause();
      }
    }

    if(coin3.getYPos() > PLAY_Y_HEIGHT) {
      coin3.resetCoin();
      coin3.reset_all(PLAY_X_START*4 + WINDOW_X/2*(rand()%100)/100.0, PLAY_Y_HEIGHT, coin_speed, 2*coin_angle_deg*(rand()%100)/100.0, coin_ax, coin_ay, paused, rtheta);
      if(rand()%10 >4){
        coin3.change_to_bomb();
      }
      else{
        coin3.change_to_coin();
      }
      if(rand()%100 <5){
        coin3.change_to_magnet();
      }
      last_coin_jump_end = currTime;
    }

    sprintf(coinScoreStr, "Coins: %d", lasso.getNumCoins());
    coinScore.setMessage(coinScoreStr);

    stepCount++;
    if(magno>=0)magno--;
    if(magno<0)unmagnet(coin_ax);
    currTime += stepTime;
    wait(stepTime);
  } // End for(;;)
  } // end life loop
}//end lvl 5
//game has ended.
  cout<<"\nTHE GAME HAS ENDED"<<endl;

} // End main_program
