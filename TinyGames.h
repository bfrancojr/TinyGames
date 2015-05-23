
// TinyGames.h

#define PONG_ENABLED 1
#define FLAPPY_ENABLED 1
#define SNAKE_ENABLED 1

#include "colors.h"
#include "SSD1331.h"

typedef struct {
  int RX;
  int RY;
  int LX;
  int LY;
  byte LB;
  byte RB;
} JoyStick;

typedef void (*voidFunc)();
typedef byte (*byteFunc)();

typedef struct {
  const char *name;
  voidFunc setup;
  byteFunc loop;
} gameEntry;

// External globals
extern SSD1331 display;
extern JoyStick joystick;
extern byte lineBuffer[];

// Flappy Birds
#if FLAPPY_ENABLED
void flappy();
byte flappyLoop();
#else
void flappy() { }
byte flappyLoop() { return 1; }
#endif

// Pong
#if PONG_ENABLED
void pong();
byte pongLoop();
#else
void pong() { }
byte pongLoop() { return 1; }
#endif

// Snake
#if SNAKE_ENABLED
void snake();
byte snakeLoop();
#else
void snake() { }
byte snakeLoop() { return 1; }
#endif

#define HSC_ADDRESS       0
#define HSV_ADDRESS       1

void getJoystick();
void displayInt(int x, int y, int val);
void displayMenu(const char *title, byte col, const char *text);
void splash(const char *text, byte col);
void blink(int x, int y, const char *text, int color);
void blinkInt(int x, int y, int val, int color);
void gameOver();
int readHighScore();
void writeHighScore(int highScore);

