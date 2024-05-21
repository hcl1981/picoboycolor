#include <EEPROM.h>
#include "hardware/pll.h"
#include "hardware/clocks.h"

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include "Seg7.h"
#include <SPI.h>

#include "tiles2.h"
#include "levels.h"
#include "sprites2.h"
#include "images.h"

#define TFT_CS        10
#define TFT_RST        9
#define TFT_DC         8

#define LEDR          14
#define LEDY          13
#define LEDG          12
#define KEY_RIGHT      2 //3x
#define KEY_DOWN       3 //4
#define KEY_LEFT       4 //1x
#define KEY_UP         1 //2
#define KEY_CENTER     0
#define SPEAKER       15

#define UP             1
#define DOWN           2
#define LEFT           3
#define RIGHT          4

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

bool evenFrame = true;
int life = 3;
int levelNum = 0;
int levelMax = 9;//9;
int stepTimeDefault = 18;
int stepTime = stepTimeDefault;
unsigned int level[12][21];
int score =  0;
bool ghostsEdible = false;
int edibleDuration = 2500;
unsigned long int edibleTime = 0;
unsigned long int timerDie = 0;
unsigned int lastTone = 0;
const int xPosGorigin = 110 + 5;
int yPosGorigin = 88 + 5;
bool blOff = true;
int xPos;
int yPos;
int direc;

int xPosG1;
int yPosG1;
int direcG1;

int xPosG2;
int yPosG2;
int direcG2;

int xPosG3;
int yPosG3;
int direcG3;
unsigned long int last;

GFXcanvas16 canvas(155, 231);

void set_sys_clock_pll(uint32_t vco_freq, uint post_div1, uint post_div2) {
  if (!running_on_fpga()) {
    clock_configure(clk_sys,
                    CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX,
                    CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB,
                    48 * MHZ,
                    48 * MHZ);

    pll_init(pll_sys, 1, vco_freq, post_div1, post_div2);
    uint32_t freq = vco_freq / (post_div1 * post_div2);

    // Configure clocks
    // CLK_REF = XOSC (12MHz) / 1 = 12MHz
    clock_configure(clk_ref,
                    CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC,
                    0,  // No aux mux
                    12 * MHZ,
                    12 * MHZ);

    // CLK SYS = PLL SYS (125MHz) / 1 = 125MHz
    clock_configure(clk_sys,
                    CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX,
                    CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS,
                    freq, freq);

    clock_configure(clk_peri,
                    0,  // Only AUX mux on ADC
                    CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB,
                    48 * MHZ,
                    48 * MHZ);
  }
}

bool check_sys_clock_khz(uint32_t freq_khz, uint *vco_out, uint *postdiv1_out, uint *postdiv_out) {
  uint crystal_freq_khz = clock_get_hz(clk_ref) / 1000;
  for (uint fbdiv = 320; fbdiv >= 16; fbdiv--) {
    uint vco = fbdiv * crystal_freq_khz;
    if (vco < 400000 || vco > 1600000) continue;
    for (uint postdiv1 = 7; postdiv1 >= 1; postdiv1--) {
      for (uint postdiv2 = postdiv1; postdiv2 >= 1; postdiv2--) {
        uint out = vco / (postdiv1 * postdiv2);
        if (out == freq_khz && !(vco % (postdiv1 * postdiv2))) {
          *vco_out = vco * 1000;
          *postdiv1_out = postdiv1;
          *postdiv_out = postdiv2;
          return true;
        }
      }
    }
  }
  return false;
}

static inline bool set_sys_clock_khz(uint32_t freq_khz, bool required) {
  uint vco, postdiv1, postdiv2;
  if (check_sys_clock_khz(freq_khz, &vco, &postdiv1, &postdiv2)) {
    set_sys_clock_pll(vco, postdiv1, postdiv2);
    return true;
  } else if (required) {
    panic("System clock of %u kHz cannot be exactly achieved", freq_khz);
  }
  return false;
}

void eepromWriteInt(int adr, int wert)
{
  byte low, high;
  low = wert & 0xFF;
  high = (wert >> 8) & 0xFF;
  EEPROM.write(adr, low);
  EEPROM.write(adr + 1, high);
  EEPROM.commit();
  return;
}

// Matthias Busse 5.2014
int eepromReadInt(int adr)
{
  byte low, high;
  low = EEPROM.read(adr);
  high = EEPROM.read(adr + 1);
  return low + ((high << 8) & 0xFF00);

  return 0;
}

void sendB() {
  if (evenFrame) {
    tft.drawRGBBitmap(40, 25, canvas.getBuffer(), canvas.width(), canvas.height());
    evenFrame = false;
  } else {
    evenFrame = true;
  }
}


void drawCentre(const char *buf, int x, int y)
{
  canvas.setRotation(1);
  canvas.setTextColor(ST77XX_WHITE);
  int16_t x1, y1;
  uint16_t w, h;
  canvas.getTextBounds(buf, 0, 0, &x1, &y1, &w, &h); // get text bounds
  canvas.setCursor(x - w / 2, y + h / 2); // set cursor to center
  canvas.print(buf);
  canvas.setRotation(4);
}

void writeHiscore(int score) {
  char cstr[14];
  sprintf(cstr, "HI-SCORE %d", score);
  drawCentre(cstr, 115, 120);
}

void writeScore(int score, int lives) {
  canvas.setRotation(1);
  char cstr[16];
  itoa(score, cstr, 10);
  canvas.setCursor(5, 16); // set cursor to center
  canvas.setTextColor(ST77XX_GREEN);
  canvas.print("Score ");
  canvas.print(cstr);
  itoa(lives, cstr, 10);
  canvas.setCursor(125, 16); // set cursor to center
  canvas.setTextColor(ST77XX_RED);
  canvas.print("Lives ");
  canvas.print(cstr);
  canvas.setRotation(4);
}


void drawTile(int i, int j, int tile) {
  for (int k = 0; k < 11; k++) {
    for (int l = 0; l < 11; l++) {
      if (tiles[tile][k][l] > 0) {
        canvas.drawPixel(135 - ((i * 11) + k), ((j * 11) + l), ST77XX_WHITE);
      }
    }
  }
}

void drawImage(const byte image[64][128]) {
  for (int k = 0; k < 64; k++) {
    for (int l = 0; l < 128; l++) {
      if (image[k][l] > 0) {
        canvas.drawPixel(113 - k , 51 + l, ST77XX_WHITE);
      }
    }
  }
}

void drawLevel() {
  if (evenFrame) {
    for (int i = 0; i < 12; i++) {
      for (int j = 0; j < 21; j++) {
        if (level[i][j] > 0) {
          drawTile(i, j, level[i][j]);
        }
      }
    }
  }
}

void loadLevel(int lev) {
  for (int i = 0; i < 12; i++) {
    for (int j = 0; j < 21; j++) {
      level[i][j] = levels[lev][i][j];
    }
  }
}

void drawPacman(int x, int y, int dire) {
  if (evenFrame) {
    for (int i = 0; i < 11; i++) {
      for (int j = 0; j < 11; j++) {
        if (millis() % 500 > 250) {
          if (pacman[0][i][j] > 0) {
            canvas.drawPixel(140 - (y + i), (x + j) - 5, ST77XX_YELLOW);
          }
        } else {
          if (pacman[dire][i][j] > 0) {
            canvas.drawPixel(140 - (y + i), (x + j) - 5, ST77XX_YELLOW);
          }
        }

      }
    }
  }
}

void drawGhost(int x, int y, uint16_t color) {
  if (evenFrame) {
    for (int i = 0; i < 11; i++) {
      for (int j = 0; j < 11; j++) {
        if (!ghostsEdible) {
          if (millis() % 604 > 302) {
            if (ghost[0][i][j] > 0) {
              canvas.drawPixel(140 - (y + i), (x + j) - 5, color );
            }
          } else {
            if (ghost[1][i][j] > 0) {
              canvas.drawPixel(140 - (y + i), (x + j) - 5, color );
            }
          }
        } else {
          if (millis() % 604 > 302) {
            if (ghost[0][i][j] > 0) {
              canvas.drawPixel(140 - (y + i), (x + j) - 5, ST77XX_BLUE );
            }
          } else {
            if (ghost[1][i][j] > 0) {
              canvas.drawPixel(140 - (y + i), (x + j) - 5, ST77XX_BLUE );
            }
          }
        }
      }
    }
  }
}

void drawEdible() {

  int pixels = (220 * (edibleDuration - (millis() - edibleTime))) / edibleDuration;
  if (pixels < 221) {
    canvas.drawLine(0, 2, 0, pixels + 2, ST77XX_WHITE);
    canvas.drawLine(1, 2, 1, pixels + 2, ST77XX_WHITE);
    canvas.drawLine(2, 2, 2, pixels + 2, ST77XX_WHITE);
  }
}

void pacmanStep() {
  if (direc == UP && ((level[((yPos - 6) / 11)][(xPos - 5) / 11] < 1) ||  (level[(yPos - 6) / 11][(xPos - 5) / 11] > 16))) {
    yPos = yPos - 1;
  }
  if (direc == DOWN && ((level[((yPos + 6) / 11)][(xPos - 5) / 11] < 1) ||  (level[(yPos + 6) / 11][(xPos - 5) / 11] > 16))) {
    yPos = yPos + 1;
  }
  if (direc == LEFT && ((level[((yPos - 5) / 11)][(xPos - 6) / 11] < 1) ||  (level[(yPos - 5) / 11][(xPos - 6) / 11] > 16))) {
    xPos = xPos - 1;
  }
  if (direc == RIGHT && ((level[((yPos - 5) / 11)][(xPos + 6) / 11] < 1) ||  (level[(yPos - 5) / 11][(xPos + 6) / 11] > 16))) {
    xPos = xPos + 1;
  }
  if (xPos == 5) {
    xPos = 221;
  } else if (xPos == 222) {
    xPos = 6;
  }
}

bool canMove(int x, int y, int direc) {
  if ((x - 5) % 11 == 0 && (y - 5) % 11 == 0) {

    if (direc == DOWN && ((level[((y + 6) / 11)][(x - 5) / 11] < 1) ||  (level[(y + 6) / 11][(x - 5) / 11] > 16))) {
      return true; //DOWN
    }

    if (direc == UP && ((level[((y - 6) / 11)][(x - 5) / 11] < 1) ||  (level[(y - 6) / 11][(x - 5) / 11] > 16))) {
      return true; //UP
    }

    if (direc == LEFT && ((level[((y - 5) / 11)][(x - 6) / 11] < 1) ||  (level[(y - 5) / 11][(x - 6) / 11] > 16))) {
      return true; //LEFT
    }

    if (direc == RIGHT && ((level[((y - 5) / 11)][(x + 6) / 11] < 1) ||  (level[(y - 5) / 11][(x + 6) / 11] > 16))) {
      return true; //RIGHT
    }

  }
  return false;
}

void ghostStep() {
  if (direcG1 == UP && ((level[((yPosG1 - 6) / 11)][(xPosG1 - 5) / 11] < 1) ||  (level[(yPosG1 - 6) / 11][(xPosG1 - 5) / 11] > 16))) {
    yPosG1 = yPosG1 - 1;
  }
  if (direcG1 == DOWN && ((level[((yPosG1 + 6) / 11)][(xPosG1 - 5) / 11] < 1) ||  (level[(yPosG1 + 6) / 11][(xPosG1 - 5) / 11] > 16))) {
    yPosG1 = yPosG1 + 1;
  }
  if (direcG1 == LEFT && ((level[((yPosG1 - 5) / 11)][(xPosG1 - 6) / 11] < 1) ||  (level[(yPosG1 - 5) / 11][(xPosG1 - 6) / 11] > 16))) {
    xPosG1 = xPosG1 - 1;
  }
  if (direcG1 == RIGHT && ((level[((yPosG1 - 5) / 11)][(xPosG1 + 6) / 11] < 1) ||  (level[(yPosG1 - 5) / 11][(xPosG1 + 6) / 11] > 16))) {
    xPosG1 = xPosG1 + 1;
  }

  if (direcG2 == UP && ((level[((yPosG2 - 6) / 11)][(xPosG2 - 5) / 11] < 1) ||  (level[(yPosG2 - 6) / 11][(xPosG2 - 5) / 11] > 16))) {
    yPosG2 = yPosG2 - 1;
  }
  if (direcG2 == DOWN && ((level[((yPosG2 + 6) / 11)][(xPosG2 - 5) / 11] < 1) ||  (level[(yPosG2 + 6) / 11][(xPosG2 - 5) / 11] > 16))) {
    yPosG2 = yPosG2 + 1;
  }
  if (direcG2 == LEFT && ((level[((yPosG2 - 5) / 11)][(xPosG2 - 6) / 11] < 1) ||  (level[(yPosG2 - 5) / 11][(xPosG2 - 6) / 11] > 16))) {
    xPosG2 = xPosG2 - 1;
  }
  if (direcG2 == RIGHT && ((level[((yPosG2 - 5) / 11)][(xPosG2 + 6) / 11] < 1) ||  (level[(yPosG2 - 5) / 11][(xPosG2 + 6) / 11] > 16))) {
    xPosG2 = xPosG2 + 1;
  }

  if (direcG3 == UP && ((level[((yPosG3 - 6) / 11)][(xPosG3 - 5) / 11] < 1) ||  (level[(yPosG3 - 6) / 11][(xPosG3 - 5) / 11] > 16))) {
    yPosG3 = yPosG3 - 1;
  }
  if (direcG3 == DOWN && ((level[((yPosG3 + 6) / 11)][(xPosG3 - 5) / 11] < 1) ||  (level[(yPosG3 + 6) / 11][(xPosG3 - 5) / 11] > 16))) {
    yPosG3 = yPosG3 + 1;
  }
  if (direcG3 == LEFT && ((level[((yPosG3 - 5) / 11)][(xPosG3 - 6) / 11] < 1) ||  (level[(yPosG3 - 5) / 11][(xPosG3 - 6) / 11] > 16))) {
    xPosG3 = xPosG3 - 1;
  }
  if (direcG3 == RIGHT && ((level[((yPosG3 - 5) / 11)][(xPosG3 + 6) / 11] < 1) ||  (level[(yPosG3 - 5) / 11][(xPosG3 + 6) / 11] > 16))) {
    xPosG3 = xPosG3 + 1;
  }
}

void pacmanSteer() {
  if (digitalRead(KEY_UP) == LOW && (canMove(xPos, yPos, UP) || direc == DOWN)) {
    direc = UP;
  }
  if (digitalRead(KEY_DOWN) == LOW && (canMove(xPos, yPos, DOWN) || direc == UP)) {
    direc = DOWN;
  }
  if (digitalRead(KEY_LEFT) == LOW && (canMove(xPos, yPos, LEFT) || direc == RIGHT)) {
    direc = LEFT;
  }
  if (digitalRead(KEY_RIGHT) == LOW && (canMove(xPos, yPos, RIGHT) || direc == LEFT)) {
    direc = RIGHT;
  }
}

bool inBox(int x, int y) {
  return ((x - 5) / 11 > 8 && (x - 5) / 11 < 12 && (y - 5) / 11 == 8);
}

void ghostSteer() {
  //return;
  // Geist 1
  if (((xPosG1 - 5) % 11 == 0 && (yPosG1 - 5) % 11 == 0)) {
    bool setG1 = true;
    while (setG1) {
      if (canMove(xPosG1, yPosG1, DOWN)) {
        if (random(1000) > 500 && setG1 && (direcG1 != UP || inBox(xPosG1, yPosG1))) {
          direcG1 = DOWN;
          setG1 = false;
          Serial.println(1);
        }
      }
      if (canMove(xPosG1, yPosG1, UP)) {
        if (random(1000) > 500 && setG1 && (direcG1 != DOWN || inBox(xPosG1, yPosG1))) {
          direcG1 = UP;
          setG1 = false;
          Serial.println(2);
        }
      }
      if (canMove(xPosG1, yPosG1, LEFT)) {
        if (xPosG1 > 221 || random(1000) > 500 && setG1 && (direcG1 != RIGHT || inBox(xPosG1, yPosG1))) {
          direcG1 = LEFT;
          setG1 = false;
          Serial.println(3);
        }
      }
      if (canMove(xPosG1, yPosG1, RIGHT)) {
        if (xPosG1 < 6 || random(1000) > 500 && setG1 && (direcG1 != LEFT || inBox(xPosG1, yPosG1))) {
          direcG1 = RIGHT;
          setG1 = false;
          Serial.println(4);
        }
      }
    }
  }
  // Geist 2

  if (((xPosG2 - 5) % 11 == 0 && (yPosG2 - 5) % 11 == 0)) {
    bool setG2 = true;
    while (setG2) {
      if (canMove(xPosG2, yPosG2, DOWN)) {
        if (random(1000) > 500 && setG2 && (direcG2 != UP || inBox(xPosG2, yPosG2))) {
          direcG2 = DOWN;
          setG2 = false;
        }
      }
      if (canMove(xPosG2, yPosG2, UP)) {
        if (random(1000) > 500 && setG2 && (direcG2 != DOWN || inBox(xPosG2, yPosG2))) {
          direcG2 = UP;
          setG2 = false;
        }
      }
      if (canMove(xPosG2, yPosG2, LEFT)) {
        if (xPosG2 > 221 || random(1000) > 500 && setG2 && (direcG2 != RIGHT || inBox(xPosG2, yPosG2))) {
          direcG2 = LEFT;
          setG2 = false;
        }
      }
      if (canMove(xPosG2, yPosG2, RIGHT)) {
        if (xPosG2 < 6 || random(1000) > 500 && setG2 && (direcG2 != LEFT || inBox(xPosG2, yPosG2))) {
          direcG2 = RIGHT;
          setG2 = false;
        }
      }
    }
  }

  // Geist 3
  if (((xPosG3 - 5) % 11 == 0 && (yPosG3 - 5) % 11 == 0)) {
    bool setG3 = true;
    while (setG3) {
      if (canMove(xPosG3, yPosG3, DOWN)) {
        if (random(1000) > 500 && setG3 && (direcG3 != UP || inBox(xPosG3, yPosG3))) {
          direcG3 = DOWN;
          setG3 = false;
        }
      }
      if (canMove(xPosG3, yPosG3, UP)) {
        if (random(1000) > 500 && setG3 && (direcG3 != DOWN || inBox(xPosG3, yPosG3))) {
          direcG3 = UP;
          setG3 = false;
        }
      }
      if (canMove(xPosG3, yPosG3, LEFT)) {
        if (xPosG3 > 221 || random(1000) > 500 && setG3 && (direcG3 != RIGHT || inBox(xPosG3, yPosG3))) {
          direcG3 = LEFT;
          setG3 = false;
        }
      }
      if (canMove(xPosG3, yPosG3, RIGHT)) {
        if (xPosG3 < 6 || random(1000) > 500 && setG3 && (direcG3 != LEFT || inBox(xPosG3, yPosG3))) {
          direcG3 = RIGHT;
          setG3 = false;
        }
      }
    }
  }
}

void collect() {
  if ((xPos - 5) % 11 == 0 && (yPos - 5) % 11 == 0) {
    if (level[(yPos - 5) / 11][((xPos - 5) / 11)] == 17) {
      level[(yPos - 5) / 11][((xPos - 5) / 11)] = 0;
      score += 1;
      //tone(SPEAKER, 6000, 10);
    } //kleine Pille
    if (level[(yPos - 5) / 11][((xPos - 5) / 11)] == 18) {
      level[(yPos - 5) / 11][((xPos - 5) / 11)] = 0;
      score += 1;
      ghostsEdible = true;
      tone(SPEAKER, 3500, 10);
      edibleTime = millis();
      //tone(SPEAKER, 4000, 10);
    } //große Pille
  }
}

void die() {

  tone(SPEAKER, 3000, 10);
  boolean hiscore = false;
  direc = 0;
  life = life - 1;
  for (int h = 0; h < 40; h++) {
    while (millis() < last + stepTime) {}
    last = millis();
    ghostSteer();
    ghostStep();
    canvas.fillScreen(0x0000);
    writeScore(score, life);
    drawLevel();
    drawEdible();
    for (int i = 0; i < 11; i++) {
      for (int j = 0; j < 11; j++) {
        if (dieAnimation[h / 4][i][j] > 0) {
          canvas.drawPixel(140 - (yPos + i), (xPos + j) - 5, ST77XX_YELLOW );
        }
      }
    }
    drawGhost(xPosG1, yPosG1, ST77XX_CYAN);
    drawGhost(xPosG2, yPosG2, ST77XX_GREEN);
    drawGhost(xPosG3, yPosG3, ST77XX_MAGENTA);
    writeScore(score, life);
    if (millis() > edibleTime + edibleDuration) {
      ghostsEdible = false;
      //digitalWrite(LEDR,LOW);
    }
    sendB();
  }
  timerDie = millis();
  while (millis() < timerDie + 4000) {
    while (millis() < last + stepTime) {}
    last = millis();
    ghostSteer();
    ghostStep();
    canvas.fillScreen(ST77XX_BLACK);
    writeScore(score, life);
    drawLevel();
    drawEdible();
    drawGhost(xPosG1, yPosG1, ST77XX_CYAN);
    drawGhost(xPosG2, yPosG2, ST77XX_GREEN);
    drawGhost(xPosG3, yPosG3, ST77XX_MAGENTA);
    writeScore(score, life);
    if (millis() > edibleTime + edibleDuration) {
      ghostsEdible = false;
      //digitalWrite(LEDR,LOW);
    }
    if ( millis() > timerDie + 1000) {
      //155,213
      canvas.fillRoundRect(25, 35, 90, 161, 5, ST77XX_BLACK);
      canvas.drawRoundRect(25, 35, 90, 161, 5, ST77XX_WHITE);
      canvas.setRotation(1);
      canvas.setTextColor(ST77XX_WHITE);
      if (life > 0) {
        if (millis() > timerDie + 3000) {
          canvas.setCursor(70, 75);
          canvas.println("TABman");
          canvas.setCursor(70, 105);
          canvas.println("died!    1");
        } else if (millis() > timerDie + 2000) {
          canvas.setCursor(70, 75);
          canvas.println("TABman");
          canvas.setCursor(70, 105);
          canvas.println("died!   2");
        } else if (millis() > timerDie + 1000) {
          canvas.setCursor(70, 75);
          canvas.println("TABman");
          canvas.setCursor(70, 105);
          canvas.println("died!   3");
        }
      }

      else {

        if (score > eepromReadInt(4) || hiscore)
        {
          eepromWriteInt(4, score);
          canvas.setCursor(70, 75);
          canvas.println("HI-SCORE");
          if (millis() % 100 > 50) {
            canvas.setCursor(70, 105);
            canvas.println(score);
          }
          hiscore = true;
        }
        else
        {
          canvas.setCursor(70, 75);
          canvas.println("No more");
          canvas.setCursor(70, 105);
          canvas.println("TABmen!");
        }
      }
    }
    canvas.setRotation(4);
    sendB();

  }
  xPos = 110 + 5;
  yPos = 66 + 5;
  direc = UP;

  xPosG1 = 110 + 5;
  yPosG1 = 88 + 5;
  direcG1 = LEFT;

  xPosG2 = 110 + 5;
  yPosG2 = 88 + 5;
  direcG2 = DOWN;

  xPosG3 = 110 + 5;
  yPosG3 = 88 + 5;
  direcG3 = RIGHT;

  direcG3 = RIGHT;
  ghostsEdible = false;
}

void handleCollision() {
  if (ghostsEdible) {
    if (abs(xPos - xPosG1) < 4 && abs(yPos - yPosG1) < 4) {
      xPosG1 = xPosGorigin;
      yPosG1 = yPosGorigin;

      tone(SPEAKER, 3500, 10);
    }
    if (abs(xPos - xPosG2) < 4 && abs(yPos - yPosG2) < 4) {
      xPosG2 = xPosGorigin;
      yPosG2 = yPosGorigin;

      tone(SPEAKER, 3500, 10);
    }
    if (abs(xPos - xPosG3) < 4 && abs(yPos - yPosG3) < 4) {
      xPosG3 = xPosGorigin;
      yPosG3 = yPosGorigin;

      tone(SPEAKER, 3500, 10);
    }
  } else {
    if (abs(xPos - xPosG1) < 4 && abs(yPos - yPosG1) < 4) {
      die();
    }
    if (abs(xPos - xPosG2) < 4 && abs(yPos - yPosG2) < 4) {
      die();
    }
    if (abs(xPos - xPosG3) < 4 && abs(yPos - yPosG3) < 4) {
      die();
    }
  }
}

boolean checkWon() {
  bool won = true;
  for (int i = 0; i < 12; i++) {
    for (int j = 0; j < 21; j++) {
      if (level[i][j] == 17 || level[i][j] == 18) {
        won = false;
      }
    }
  }
  return won;
}



void run2(void) {
  last = millis();
  canvas.setFont(NULL);
  canvas.fillScreen(ST77XX_BLACK);
  drawImage(titlePic);
  writeHiscore(eepromReadInt(4));
  sendB();

  if (blOff) {
    for (int i = 0; i < 100; i++) {
      analogWrite(26, i);
      delay(18);
    }
  }
  blOff = false;

  while (digitalRead(KEY_CENTER) == HIGH || millis() < last + 500) {
    canvas.setRotation(1);
    if (millis() % 600 > 300) {
      canvas.setTextColor(ST77XX_WHITE);
      canvas.setCursor(108, 95);
      canvas.println("PUSH TO PLAY");
    } else {
      canvas.setTextColor(ST77XX_BLACK);
      canvas.setCursor(108, 95);
      canvas.println("PUSH TO PLAY");
    }
    canvas.setRotation(4);
    sendB();
    Serial.println(millis());
  }

  canvas.setFont(&Seven_Segment12pt7b);
  delay(500);
  while (life > 0 && stepTime >= 8) {
    while (levelNum <= levelMax && life > 0) {
      if (life > 0)loadLevel(levelNum);
      xPos = 110 + 5;
      yPos = 66 + 5;
      direc = UP;

      xPosG1 = 110 + 5;
      yPosG1 = 88 + 5;
      direcG1 = LEFT;

      xPosG2 = 110 + 5;
      yPosG2 = 88 + 5; //88 + 5;
      direcG2 = DOWN;

      xPosG3 = 110 + 5;
      yPosG3 = 88 + 5;
      direcG3 = RIGHT;

      while (!checkWon() && life > 0) {
        while (millis() < last + stepTime) {}
        last = millis();
        pacmanSteer();
        pacmanStep();
        ghostSteer();
        ghostStep();
        collect();

        handleCollision();
        canvas.fillScreen(ST77XX_BLACK);
        writeScore(score, life);
        drawLevel();
        drawEdible();
        if (life > 0) {
          drawPacman(xPos, yPos, direc);
        }
        drawGhost(xPosG1, yPosG1, ST77XX_CYAN);
        drawGhost(xPosG2, yPosG2, ST77XX_GREEN);
        drawGhost(xPosG3, yPosG3, ST77XX_MAGENTA);
        writeScore(score, life);
        sendB();

        if (millis() > edibleTime + edibleDuration) {
          ghostsEdible = false;
          //digitalWrite(LEDR,LOW);
          //digitalWrite(LEDG,LOW);
          //digitalWrite(LEDY,LOW);
        } else {
          /*
            if(millis() > (edibleTime + edibleDuration)-500){

            if(digitalRead(LEDR)==LOW){
              digitalWrite(LEDR,HIGH);
              //digitalWrite(LEDG,HIGH);
              //digitalWrite(LEDY,HIGH);
              } else {
              digitalWrite(LEDR,LOW);
              //digitalWrite(LEDG,LOW);
              //digitalWrite(LEDY,LOW);
            }
            }
          */
          if ((millis() > lastTone + 50) && millis() > (edibleTime + edibleDuration) - 50) {

            tone(SPEAKER, 5000, 10);
            lastTone = millis();

          }
        }
      }
      levelNum += 1;
    }
    stepTime -= 2;
    edibleDuration -= 200;
    levelNum = 0;
  }
  if (life == 0) {
    int last2 = millis();
    while (millis() < last2 + 2000) {
      while (millis() < last + stepTime) {}
      last = millis();
      ghostSteer();
      ghostStep();
      canvas.fillScreen(ST77XX_BLACK);
      writeScore(score, life);
      drawLevel();
      drawEdible();
      drawGhost(xPosG1, yPosG1, ST77XX_CYAN);
      drawGhost(xPosG2, yPosG2, ST77XX_GREEN);
      drawGhost(xPosG3, yPosG3, ST77XX_MAGENTA);
      writeScore(score, life);
      sendB();
      if (millis() > edibleTime + edibleDuration) {
        ghostsEdible = false;
        digitalWrite(LEDR, LOW);
        //digitalWrite(LEDG,LOW);
        //digitalWrite(LEDY,LOW);
      }
    }
  } else {
    canvas.fillScreen(ST77XX_BLACK);
    drawImage(wonPic);
    sendB();
    delay(3000);
  }
  //vorbei..
}
void run() {
  //u8g2.setFontDirection(1);
  while (true)
  {
    run2();
    life = 3;
    levelNum = 0;
    stepTime = stepTimeDefault;
    score =  0;
    bool ghostsEdible = false;
  }
}


void setup() {
  set_sys_clock_khz(160000, true);
  tft.init(240, 280);
  tft.setSPISpeed(625000000);
  //tft.setSPISpeed(175000000);
  tft.setRotation(4);

  //tft.setFont(&Seven_Segment12pt7b);
  tft.setTextColor(ST77XX_WHITE);
  tft.fillScreen(ST77XX_BLACK);

  //canvas.setFont(&Seven_Segment12pt7b);
  canvas.setTextColor(ST77XX_WHITE);
  canvas.fillScreen(ST77XX_BLACK);

  pinMode(26, OUTPUT);
  analogWrite(26, 0);

  pinMode(KEY_RIGHT, INPUT_PULLUP);
  pinMode(KEY_DOWN, INPUT_PULLUP);
  pinMode(KEY_LEFT, INPUT_PULLUP);
  pinMode(KEY_UP, INPUT_PULLUP);
  pinMode(KEY_CENTER, INPUT_PULLUP);

  pinMode(SPEAKER, OUTPUT);
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDY, OUTPUT);

  EEPROM.begin(256);

  if (eepromReadInt(4) >= 65500)
  {
    eepromWriteInt(4, 0);
  }

  run();
}

void loop() {}
