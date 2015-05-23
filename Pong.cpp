
// PONG

#include "TinyGames.h"

enum {
  ballH = 4,
  ballW = 4,
  paddleW = 3,
  paddleH = 14,
  playerOffsetX = 0,
  enemyOffsetX = 92
};

static const char *selections[] = {
  "One Player",
  "Two Players"
};

static byte ballOffsetX;
static byte ballOffsetY;

static char playerOffsetY;
static char enemyOffsetY;

static char players;
static char playersSelection;

static char ballVelX;
static char ballVelY;

static byte scoreLeft;
static byte scoreRight;

static byte framecount;

static void updateDisplay()
{
  display.goTo(0, 0);
  display.startData();

  for (int y = 0; y < 64; y++)
  {
    for (int x = 0; x < 96; x++)
    {
      if (y < 1
          || y > 62
          || (x > ballOffsetX && x < ballOffsetX + ballW && y > ballOffsetY
              && y < ballOffsetY + ballH)
          || (x >= playerOffsetX && x < playerOffsetX + paddleW && y > playerOffsetY
              && y < playerOffsetY + paddleH)
          || (x >= enemyOffsetX && x < enemyOffsetX + paddleW && y > enemyOffsetY
              && y < enemyOffsetY + paddleH)
          || (x == 48 && !(y % 3)))
      {
        lineBuffer[x] = GREEN;
      } else {
        lineBuffer[x] = BLACK;
      }
    }
    display.writeBuffer(lineBuffer, 96);
  }
  display.endTransfer();
}

static void menu()
{
  displayMenu("Pong", 28, selections[playersSelection]);
  ballOffsetX = 46;
  ballOffsetY = 30;
  ballVelX = -1;
  ballVelY = scoreLeft = scoreRight = players = playersSelection = framecount = 0;
  playerOffsetY = enemyOffsetY = (64 - paddleH) / 2;
}

void pong()
{
  splash("Pong", 30);
  menu();
}

byte pongLoop()
{
  getJoystick();

  if (players == 0)
  {
    int LY = joystick.LY;
    int LX = joystick.LX;

    if (abs(LY) > 200)
    {
      playersSelection ^= 1;
      menu();

      do {
        delay(50);
        getJoystick();
      } while (abs(LY - joystick.LY) < 20);
      delay(200);
      return 0;
    }

    if (abs(LX) > 200)
    {
      delay(200);
      players = playersSelection + 1;
    }
    return 0;
  }
  else
  {
    if (ballVelX == 0)
    {
      ballVelX = int(random(-4, 4));
    }
    if (ballVelY == 0)
    {
      ballVelY = int(random(-4, 4));
    }

    if ((ballOffsetX == 3 && ballOffsetY > playerOffsetY - paddleW
         && ballOffsetY < playerOffsetY + paddleH + paddleW) ||
        (ballOffsetX == 88 && ballOffsetY > enemyOffsetY - paddleW
         && ballOffsetY < enemyOffsetY + paddleH + paddleW))
    {
      ballVelX = -ballVelX;
      ballVelY = int(ballVelY + random(-2, 2));
    }

    ballVelX = min(4, ballVelX);
    ballVelX = max(-4, ballVelX);

    ballVelY = min(4, ballVelY);
    ballVelY = max(-4, ballVelY);

    ballOffsetX += ballVelX;
    ballOffsetY += ballVelY;

    if (ballOffsetX > 91 || ballOffsetX < 1)
    {
      if (ballOffsetX < 1)
      {
        ++scoreLeft;
      }
      else
      {
        ++scoreRight;
      }
      display.setFont(liberationSans_12ptFontInfo);
      display.fontColor(WHITE, BLACK);
      displayInt(28, 5, scoreLeft);
      displayInt(64, 5, scoreRight);
      if (scoreLeft > 4 || scoreRight > 4) {
        gameOver();
        return 1;
      } else {
        blink(28, 25, "GOAL!", WHITE);
      }
      ballOffsetX = 46;
      ballOffsetY = 30;
      ballVelX = 1 - 2 * (random(-10, 10) > 0);
      ballVelY = 0;
      return 0;
    }

    playerOffsetY = (-joystick.LY + 370) / 14;
    playerOffsetY = max(1, playerOffsetY);
    playerOffsetY = min(62 - paddleH, playerOffsetY);

    updateDisplay();

    if (players == 2)
    {
      enemyOffsetY = (-joystick.RY + 280) / 13;
      enemyOffsetY = max(1, enemyOffsetY);
    }
    else
    {
      framecount++;
      if (enemyOffsetY > ballOffsetY - 6)
      {
        enemyOffsetY--;
        if (framecount % 2)
        {
          enemyOffsetY--;
        }
      }
      enemyOffsetY = max(1, enemyOffsetY);
      if (enemyOffsetY < ballOffsetY - 6)
      {
        enemyOffsetY++;
        if (framecount % 2)
        {
          enemyOffsetY++;
        }
      }
    }
    enemyOffsetY = min(62 - paddleH, enemyOffsetY);

    if (ballOffsetY <= 1 || ballOffsetY >= 60)
    {
      ballVelY = -ballVelY;
    }
  }
  return 0;
}
