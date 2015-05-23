
// Flappy Birds

#include "TinyGames.h"

static const byte PROGMEM flappyBird[] =
{
  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  BLACK,  BLACK,  BLACK,
  BLACK,  BLACK,  BLACK,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,
  ALPHA,  ALPHA,  ALPHA,  BLACK,  BLACK,  WHITE,  WHITE,  WHITE,  BLACK,
  WHITE,  WHITE,  BLACK,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,
  ALPHA,  BLACK,  WHITE,  WHITE,  YELLOW, YELLOW, BLACK,  WHITE,  WHITE,
  WHITE,  WHITE,  BLACK,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  BLACK,
  WHITE,  YELLOW, YELLOW, YELLOW, YELLOW, BLACK,  WHITE,  WHITE,  WHITE,
  BLACK,  WHITE,  BLACK,  ALPHA,  ALPHA,  ALPHA,  BLACK,  YELLOW, YELLOW,
  YELLOW, YELLOW, YELLOW, YELLOW, BLACK,  WHITE,  WHITE,  WHITE,  BLACK,
  WHITE,  BLACK,  ALPHA,  ALPHA,  ALPHA,  BLACK,  YELLOW, YELLOW, YELLOW,
  YELLOW, YELLOW, YELLOW, YELLOW, BLACK,  WHITE,  WHITE,  WHITE,  WHITE,
  BLACK,  ALPHA,  ALPHA,  ALPHA,  BLACK,  YELLOW, YELLOW, YELLOW, YELLOW,
  YELLOW, YELLOW, YELLOW, YELLOW, BLACK,  BLACK,  BLACK,  BLACK,  BLACK,
  BLACK,  ALPHA,  ALPHA,  BLACK,  YELLOW, YELLOW, YELLOW, YELLOW, YELLOW,
  YELLOW, YELLOW, BLACK,  RED,    RED,    RED,    RED,    RED,    RED,
  BLACK,  ALPHA,  ALPHA,  BLACK,  YELLOW, YELLOW, YELLOW, YELLOW, YELLOW,
  YELLOW, BLACK,  RED,    BLACK,  BLACK,  BLACK,  BLACK,  BLACK,  BLACK,
  ALPHA,  ALPHA,  BLACK,  YELLOW, YELLOW, YELLOW, YELLOW, YELLOW, YELLOW,
  BLACK,  RED,    RED,    RED,    RED,    RED,    BLACK,  ALPHA,  ALPHA,
  ALPHA,  ALPHA,  BLACK,  BLACK,  YELLOW, YELLOW, YELLOW, YELLOW, YELLOW,
  BLACK,  BLACK,  BLACK,  BLACK,  BLACK,  ALPHA,  ALPHA,  ALPHA,  ALPHA,
  ALPHA,  ALPHA,  ALPHA,  BLACK,  BLACK,  BLACK,  BLACK,  BLACK,  ALPHA,
  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA
};

//7x8
static const byte PROGMEM wingUp[] =
{
  ALPHA,  BLACK,  BLACK,  BLACK,  BLACK,  ALPHA,  ALPHA,
  BLACK,  WHITE,  WHITE,  WHITE,  WHITE,  BLACK,  ALPHA,
  BLACK,  WHITE,  WHITE,  WHITE,  WHITE,  WHITE,  BLACK,
  BLACK,  YELLOW, WHITE,  WHITE,  WHITE,  YELLOW, BLACK,
  ALPHA,  BLACK,  YELLOW, YELLOW, YELLOW, BLACK,  ALPHA,
  ALPHA,  ALPHA,  BLACK,  BLACK,  BLACK,  ALPHA,  ALPHA,
  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,
  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,
};

static const byte PROGMEM wingMid[] =
{
  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,
  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,
  ALPHA,  BLACK,  BLACK,  BLACK,  BLACK,  BLACK,  ALPHA,
  BLACK,  WHITE,  WHITE,  WHITE,  WHITE,  WHITE,  BLACK,
  BLACK,  YELLOW, WHITE,  WHITE,  WHITE,  YELLOW, BLACK,
  ALPHA,  BLACK,  BLACK,  BLACK,  BLACK,  BLACK,  ALPHA,
  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,
  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,
};

static const byte PROGMEM wingDown[] =
{
  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,
  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,
  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,  ALPHA,
  ALPHA,  BLACK,  BLACK,  BLACK,  BLACK,  BLACK,  ALPHA,
  BLACK,  YELLOW, WHITE,  WHITE,  WHITE,  YELLOW, BLACK,
  BLACK,  WHITE,  WHITE,  WHITE,  WHITE,  BLACK,  ALPHA,
  BLACK,  WHITE,  WHITE,  YELLOW, BLACK,  ALPHA,  ALPHA,
  ALPHA,  BLACK,  BLACK,  BLACK,  BLACK,  ALPHA,  ALPHA,
};

enum {
  pipeWidth = 10,
  pipeSpacingX = 26,
  pipeSpacingY = 26,
  cloudWidth = 15,
  cloudSpacingX = 30
};

static char wingHeight = 8;
static char wingWidth = 7;

static char flappyBirdX = 25;
static char flappyBirdY = 30;

static char flappyBirdHeight = 12;
static char flappyBirdWidth = 17;

static char amtPipes = 4;

static char PipeHeight[] = {10, 15, 20, 25, 25, 10, 10};
static char PipeColor[] = {GREEN, GREEN, GREEN, GREEN, GREEN, GREEN, GREEN};
static char pipeOffsetX = 0;
static char collided = 0;

static byte BGcolor = BLUE;

static char wingPos = 1;

static char cloudY[] = {5, 15, 30, 25, 5, 0};
static char cloudOffsetX = 0;
static byte cloudColor = WHITE;

static char groundY = 52;
static char groundOffsetX = 0;

static char birdYmod = 1;

static byte framecount = 0;

static char brightnessChanged = 0;
static char brightness = 5;

static char movePipe = 4;
static char movePipeMod = 1;

static byte collisions;

static void setBuffer(char i, char amt, byte color)
{
  char endbyte = i + amt;
  while (i < endbyte)
  {
    lineBuffer[i++] = color;
  }
}

static void updateDisplay()
{
  display.goTo(0, 0);
  display.startData();
  for (byte y = 0; y < groundY; y++)
  {
    for (byte i = 16; i < 112; i++)
    {
      lineBuffer[i] = BGcolor;
    }
    int x = cloudOffsetX;
    char cloud = 0;

    // Displays clouds
    while (x < 16 + 96)
    {
      if (y > cloudY[cloud] && y < cloudY[cloud] + 8)
      {
        if (y < cloudY[cloud] + 2 || y > cloudY[cloud] + 6)
        {
          setBuffer(x + 1, cloudWidth - 2, cloudColor);
        }
        else
        {
          setBuffer(x, cloudWidth, cloudColor);
        }
      }
      cloud++;
      x += (cloudSpacingX + cloudWidth);
    }

    x = pipeOffsetX;
    char pipe = 0;

    // Displays pipes
    while (x < 16 + 96)
    {
      if (y < PipeHeight[pipe] || y > PipeHeight[pipe] + pipeSpacingY)
      {
        if (y < PipeHeight[pipe] - 4
            || y > PipeHeight[pipe] + pipeSpacingY + 4)
        {
          setBuffer(x, pipeWidth, PipeColor[pipe]);
        }
        else
        {
          setBuffer(x - 1, pipeWidth + 2, PipeColor[pipe]);
        }
      }
      pipe++;
      x += (pipeSpacingX + pipeWidth);
    }

    // Displays bird body
    if (y >= flappyBirdY && y < flappyBirdY + flappyBirdHeight)
    {
      int offset = (y - flappyBirdY) * flappyBirdWidth;
      for (int i = 0; i < flappyBirdWidth; i++)
      {
        byte color = pgm_read_byte(flappyBird + offset + i);
        if (color != ALPHA)
        {
          lineBuffer[16 + flappyBirdX + i] = color;
        }
      }
    }
    char wingY = flappyBirdY + 3;

    // Displays bird wing
    if (y >= wingY && y < wingY + wingHeight)
    {
      int offset = (y - wingY) * wingWidth;
      for (int i = 0; i < wingWidth; i++)
      {
        byte color;
        if (wingPos == 0)
        {
          color = pgm_read_byte(wingUp + offset + i);
        }
        if (wingPos == 1)
        {
          color = pgm_read_byte(wingMid + offset + i);
        }
        if (wingPos == 2)
        {
          color = pgm_read_byte(wingDown + offset + i);
        }
        if (color != ALPHA)
        {
          lineBuffer[16 + flappyBirdX + i] = color;
        }
      }
    }
    display.writeBuffer(lineBuffer + 16, 96);
  }

  // Grass movement
  for (int z = 0; z < 21; z++)
  {
    for (byte i = 16; i < 21; ++i)
    {
      lineBuffer[i + (z * 5)] = GREEN;
    }
  }
  for (byte i = 16; i < 21; ++i)
  {
    display.writeBuffer(lineBuffer + i + groundOffsetX, 96);
  }

  // Ground
  for (byte i = 16; i < 112; i++)
  {
    lineBuffer[i] = BROWN;
  }
  for (byte i = 0; i < 7; i++)
  {
    display.writeBuffer(lineBuffer + 16, 96);
  }
}

static void collision()
{
  updateDisplay();
  display.off();
  delay(150);
  display.on();
  blink(20, 15, "CRASH!", RED);
  updateDisplay();
  ++collisions;
}

void flappy()
{
  splash("Flappy Birds", 5);
  collisions = 0;
}

byte flappyLoop()
{
  updateDisplay();

  // Red when pipe is hit
  if (pipeOffsetX < 18 && pipeOffsetX > 6)
  {
    if (flappyBirdY < (PipeHeight[1]) || flappyBirdY > (PipeHeight[1] + pipeSpacingY - 13))
    {
      if (PipeColor[1] == GREEN) {
        PipeColor[1] = RED;
        collision();
      }
    } else {
      PipeColor[1] = GREEN;
    }
  }

  framecount++;
  getJoystick();

  // Fast wing flap, move bird up
  if (joystick.LY > 200 || joystick.RY > 200)
  {
    wingPos = (framecount) % 3;
    flappyBirdY -= (birdYmod * 2);
    if (flappyBirdY < 0)
    {
      flappyBirdY = 0;
    }
  }
  else // normal flap, move bird down
  {
    wingPos = (framecount >> 2) % 3;
    flappyBirdY += birdYmod;
    if (flappyBirdY > 40)
    {
      flappyBirdY = 40;
    }
  }

  pipeOffsetX -= 1;

  // Moves pipe by pipe width(roughly)
  if (movePipe && movePipe < 5)
  {
    PipeHeight[movePipe - 1] += movePipeMod;
    if (PipeHeight[movePipe - 1] < 1)
    {
      movePipeMod = abs(movePipeMod);
    }
    if (PipeHeight[movePipe - 1] > groundY - 3 - pipeSpacingY)
    {
      movePipeMod = -1 * abs(movePipeMod);
    }
  }

  // Smooths out pipe movement
  if (pipeOffsetX <= 0)
  {
    pipeOffsetX = (pipeSpacingX + pipeWidth);
    memcpy((void *)&PipeHeight[0], (void *)&PipeHeight[1], sizeof(PipeHeight) - sizeof(PipeHeight[0]));
    PipeHeight[4] = 3 + micros() % (groundY - 8 - pipeSpacingY);
    PipeColor[1] = GREEN;
    if (movePipe)
    {
      movePipe -= 1;
    }
    else
    {
      movePipe = 3 + micros() % 5;
    }
  }

  groundOffsetX += 1;
  if (groundOffsetX >= 5)
  {
    groundOffsetX = 0;
  }

  // Moves clouds
  if (!(framecount % 2))
  {
    cloudOffsetX--;
  }
  if (cloudOffsetX <= 0)
  {
    cloudOffsetX = cloudSpacingX + cloudWidth;
    memcpy((void *)&cloudY[0], (void *)&cloudY[1], sizeof(cloudY) - sizeof(cloudY[0]));
    cloudY[5] = micros() % (30);
  }
  if (collisions > 19) {
    gameOver();
    return 1;
  }
}

