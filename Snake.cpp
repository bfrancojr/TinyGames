
// Snake

#include "TinyGames.h"

enum {
  UP,
  DOWN,
  RIGHT,
  LEFT
};

#define SCREEN_WIDTH      96
#define SCREEN_HEIGHT     64
// 8 until find out a better storage strategy
#define OBJECTS_SIZE      4  
#define SCREEN_FACTOR_X   (SCREEN_WIDTH / OBJECTS_SIZE)
#define SCREEN_FACTOR_Y   (SCREEN_HEIGHT / OBJECTS_SIZE)

#define MAX_TURNS 20

typedef struct {
  byte x;
  byte y;
} PointXY;

typedef struct {
  PointXY p1;
  PointXY p2;
  PointXY *pos;
} VisitorData;

typedef byte (*VisitorFunc)(VisitorData *);

static byte first;
static byte last;

static PointXY food;

static PointXY head;

static byte frameCount;

static byte segmentsCount;

static int highScore = 0;
static int lenght;

static byte turns[MAX_TURNS / 4]; // 2 bits each
static byte segments[MAX_TURNS];
static byte reflection[(SCREEN_FACTOR_X * SCREEN_FACTOR_Y) / sizeof(byte)]; // bit array

static byte getTurn(byte idx)
{
  return (turns[idx/4] >> (2 * (idx % 4))) & 0x03;
}

static void setTurn(byte idx, byte dir)
{
  turns[idx/4] = (turns[idx/4] & ~(0x03 << 2 * (idx % 4))) | ((dir & 0x03) << (2 * (idx % 4)));
}

static byte getSegment(byte idx)
{
  return segments[idx];
}

static void setSegment(byte idx, byte len)
{
  segments[idx] = len;
}

static void addTurn(byte dir)
{
  if (segmentsCount >= MAX_TURNS-1) {
    return;
  }
  ++segmentsCount;
  setTurn(first, dir);
  setSegment(first, 0);
  ++first;
  if (first >= MAX_TURNS) {
    first = 0;
  }
}

static byte getSnakeDir()
{
  return getTurn((first == 0 ? MAX_TURNS : first) - 1);
}

static void expand()
{
  byte head = (first == 0 ? MAX_TURNS : first) - 1;
  setSegment(head, getSegment(head) + 1);
}

static byte isLastOne()
{
  byte pos = last + 1;
  
  if (pos >= MAX_TURNS) {
    pos = 0;
  }
  return last == first || pos == first;
}

static void shrink()
{
  setSegment(last, getSegment(last) - 1);
  if (!isLastOne()) {
    if (getSegment(last) == 0) {
      --segmentsCount;
      ++last;
      if (last >= MAX_TURNS) {
        last = 0;
      }
    }
  }
}

static byte bodyWalk(VisitorFunc visitor, PointXY *data)
{
  byte segment;
  VisitorData dt;
  byte pos = first;
  PointXY turn;

  dt.pos = data;
  turn.x = head.x;
  turn.y = head.y;
  while (pos != last) {
    pos = ((pos == 0) ? MAX_TURNS : pos) - 1;
    segment = getSegment(pos);
    dt.p1.x = dt.p2.x = turn.x;
    dt.p1.y = dt.p2.y = turn.y;
    switch (getTurn(pos)) {
      case RIGHT:
        dt.p1.x -= segment;
        turn.x = dt.p1.x;
        break;
      case LEFT:
        dt.p2.x += segment;
        turn.x = dt.p2.x;
        break;
      case UP:
        dt.p2.y += segment;
        turn.y = dt.p2.y;
        break;
      case DOWN:
        dt.p1.y -= segment;
        turn.y = dt.p1.y;
        break;
    }
    if (visitor(&dt)) {
      return 1;
    }
  }
  return 0;
}

static byte intersectVisitor(VisitorData *dt)
{
  return   dt->pos->x >= dt->p1.x
        && dt->pos->x <= dt->p2.x
        && dt->pos->y >= dt->p1.y
        && dt->pos->y <= dt->p2.y;
}

static byte intersect(PointXY *pos)
{
  return bodyWalk(intersectVisitor, pos);
}

static byte updateReflectionVisitor(VisitorData *dt)
{
  int offset;

  for (byte y = dt->p1.y; y <= dt->p2.y; ++y) {
    for (byte x = dt->p1.x; x <= dt->p2.x; ++x) {
      offset = y * SCREEN_FACTOR_X + x;
      reflection[offset / sizeof(byte)] |= 1 << (offset % sizeof(byte));
    }
  }
  return 0;
}

static void updateReflection()
{
  memset(&reflection, '\0', sizeof(reflection));
  (void)bodyWalk(updateReflectionVisitor, NULL);
}

static void newFood()
{
  do {
    food.x = random(2, SCREEN_FACTOR_X-2);
    food.y = random(2, SCREEN_FACTOR_Y-2);
  } while (intersect(&food));
}

static void snakeBorn()
{
  memset(&turns, '\0', sizeof(turns));
  memset(&segments, '\0', sizeof(segments));
  first = last = segmentsCount = frameCount = lenght = 0;
  addTurn(RIGHT);
  lenght = 4;
  setSegment(0, lenght);
  head.x = head.y = 4;
  updateReflection();
  newFood();
}

void snake()
{
  splash("Snake", 25);
  highScore = readHighScore();
  snakeBorn();
}

static void newHighScore()
{
  display.cls();
  display.setFont(liberationSans_12ptFontInfo);
  display.fontColor(WHITE, BLACK);
  display.setCursor(15, 1);
  display.print(F("Congrats!"));
  display.setFont(liberationSans_8ptFontInfo);
  display.setCursor(10, 21);
  display.fontColor(GREEN, BLACK);
  display.print(F("New High Score!"));
  display.setFont(liberationSans_12ptFontInfo);
  blinkInt(35, 40, highScore, YELLOW);
  delay(3000);
}

static void endGame()
{
  display.off();
  delay(150);
  display.on();
  if (lenght > highScore) {
    highScore = lenght;
    writeHighScore(highScore);
    newHighScore();
  } else {
    displayInt(28, 5, lenght);
    gameOver();
  }
  snakeBorn();
}

static byte moveSnake()
{
  PointXY pos;
  
  pos.x = head.x;
  pos.y = head.y;
  
  switch (getSnakeDir()) {
    case RIGHT:
      ++pos.x;
      break;
    case LEFT:
      --pos.x;
      break;
    case UP:
      --pos.y;
      break;
    case DOWN:
      ++pos.y;
      break;
  }
  expand();
  shrink();
  if (pos.x < 1 || pos.x > SCREEN_FACTOR_X-2
      || pos.y < 1 || pos.y > SCREEN_FACTOR_Y-2
      || intersect(&pos))
  {
    endGame();
    return 0;
  }
  head.x = pos.x;
  head.y = pos.y;
  return 1;
}

static void updateDisplay()
{
  int offset;

  updateReflection();
  display.goTo(0, 0);
  display.startData();

  for (int y = 0; y < SCREEN_HEIGHT; y++) {
    for (int x = 0; x < SCREEN_WIDTH; x++) {
      if (y == 0 || y == SCREEN_HEIGHT-1 || x == 0 || x == SCREEN_WIDTH-1) {
        lineBuffer[x] = WHITE;
      } else {
        if (x >= food.x * OBJECTS_SIZE && x < food.x * OBJECTS_SIZE + OBJECTS_SIZE
            && y >= food.y * OBJECTS_SIZE && y < food.y * OBJECTS_SIZE + OBJECTS_SIZE) {
          lineBuffer[x] = YELLOW;
        } else {
          offset = (y / OBJECTS_SIZE) * SCREEN_FACTOR_X + (x / OBJECTS_SIZE);
          if (reflection[offset / sizeof(byte)] & (1 << (offset % sizeof(byte)))) {
            lineBuffer[x] = RED;
          } else {
            lineBuffer[x] = BLACK;
          }
        }
      }
    }
    display.writeBuffer(lineBuffer, SCREEN_WIDTH);
  }
  display.endTransfer();
}

static byte getPlayerDir()
{
  byte snakeDir = getSnakeDir();
  
  getJoystick();

  if (joystick.LX > 200 || joystick.RX > 200) {
    return (snakeDir != LEFT) ? RIGHT : snakeDir;
  }
  if (joystick.LX < -200 || joystick.RX < -200) {
    return (snakeDir != RIGHT) ? LEFT : snakeDir;
  }
  if (joystick.LY < -200 || joystick.RY < -200) {
    return (snakeDir != UP) ? DOWN : snakeDir;
  }
  if (joystick.LY > 200 || joystick.RY > 200) {
    return (snakeDir != DOWN) ? UP : snakeDir;
  }
  return snakeDir;
}

byte snakeLoop()
{
  ++frameCount;

  if (frameCount % 50 != 0) {
    return 0;
  }
  byte snakeDir = getSnakeDir();
  byte playerDir = getPlayerDir();
  if (playerDir != snakeDir) {
    addTurn(playerDir);
  }
  if (!moveSnake()) {
    return 1;
  }
  if (food.x == head.x && food.y == head.y) {
    ++lenght;
    expand();
    newFood();
  }
  updateDisplay();
  return 0;
}

