#include <Adafruit_GFX.h>
#include <Fonts/FreeSansBold12pt7b.h>

#include "title.h"
#include "back.h"
#include "top.h"

#define UP             1
#define DOWN           2
#define LEFT           3
#define RIGHT          4

#define DARKTILE       0xA689
#define BRIGHTTILE     0xAEAA
#define SNACOL         0x03fb
#define BORDERCOL      0x9a85

#define XSIZE 15
#define YSIZE 28


class Sclange{
	private:
	bool bow [8][8] = {
		{false, false, false, false, false, false, false, false},
		{false, false, false, false, false, false, false, false},
		{false, false, false, false, false, false,  true,  true},
		{false, false, false, false,  true,  true,  true,  true},
		{false, false, false,  true,  true,  true,  true,  true},
		{false, false, false,  true,  true,  true,  true,  true},
		{false, false,  true,  true,  true,  true,  true, false},
		{false, false,  true,  true,  true,  true, false, false}
	};
	
	bool tail [8][8] = {
		{false, false, false, false,  true, false, false, false},
		{false, false, false,  true, false, false, false, false},
		{false, false, false,  true,  true, false, false, false},
		{false, false, false,  true,  true, false, false, false},
		{false, false, false,  true,  true,  true, false, false},
		{false, false,  true,  true,  true,  true, false, false},
		{false, false,  true,  true,  true,  true, false, false},
		{false, false,  true,  true,  true,  true, false, false}
	};
	
	
	uint16_t head [8][8] = {
		{0x0000, 0x0000, 0x0000, SNACOL, SNACOL, 0x0000, 0x0000, 0x0000},
		{0x0000, 0x0000, SNACOL, SNACOL, SNACOL, SNACOL, 0x0000, 0x0000},
		{0x0000, 0xffff, 0xffff, SNACOL, SNACOL, 0xffff, 0xffff, 0x0000},
		{SNACOL, 0xffff, 0xf800, SNACOL, SNACOL, 0xf800, 0xffff, SNACOL},
		{SNACOL, SNACOL, SNACOL, SNACOL, SNACOL, SNACOL, SNACOL, SNACOL},
		{SNACOL, SNACOL, SNACOL, SNACOL, SNACOL, SNACOL, SNACOL, SNACOL},
		{0x0000, SNACOL, SNACOL, SNACOL, SNACOL, SNACOL, SNACOL, 0x0000},
		{0x0000, SNACOL, SNACOL, SNACOL, SNACOL, SNACOL, SNACOL, 0x0000},
	};
	
	uint16_t apple [8][8] = {
		{0x0000, 0x0000, 0x7800, 0x7800, 0x0000, 0x0000, 0x0000, 0x0000},
		{0x0000, 0xF800, 0xF800, 0x7800, 0x7800, 0xF800, 0xF800, 0x0000},
		{0x0000, 0xF800, 0xF800, 0xF800, 0x7800, 0xF800, 0xF800, 0x0000},
		{0xF800, 0xF800, 0xFB8E, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800},
		{0xF800, 0xF800, 0xFB8E, 0xFB8E, 0xF800, 0xF800, 0xF800, 0xF800},
		{0x0000, 0xF800, 0xF800, 0xFB8E, 0xF800, 0xF800, 0xF800, 0x0000},
		{0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x0000},
		{0x0000, 0x0000, 0x0000, 0xF800, 0xF800, 0x0000, 0x0000, 0x0000}
	}
	;
	
	uint16_t plum [8][8] = {
		{0x0000, 0x0000, 0x7800, 0x7800, 0x0000, 0x0000, 0x0000, 0x0000},
		{0x0000, 0xb01f, 0xb01f, 0x7800, 0x7800, 0xb01f, 0xb01f, 0x0000},
		{0x0000, 0xb01f, 0xb01f, 0xb01f, 0x7800, 0xb01f, 0xb01f, 0x0000},
		{0xb01f, 0xb01f, 0xcb7f, 0xb01f, 0xb01f, 0xb01f, 0xb01f, 0xb01f},
		{0xb01f, 0xb01f, 0xcb7f, 0xcb7f, 0xb01f, 0xb01f, 0xb01f, 0xb01f},
		{0x0000, 0xb01f, 0xb01f, 0xcb7f, 0xb01f, 0xb01f, 0xb01f, 0x0000},
		{0x0000, 0xb01f, 0xb01f, 0xb01f, 0xb01f, 0xb01f, 0xb01f, 0x0000},
		{0x0000, 0x0000, 0x0000, 0xb01f, 0xb01f, 0x0000, 0x0000, 0x0000}
	}
	;
	
	unsigned long int plumtime = 0;
	int addScore = 0;
	bool dispoff = true;
	Adafruit_ST7789 tft;
	
	GFXcanvas16 canvas;
	GFXcanvas16 canvas2;
	GFXcanvas16 canvas3;
	
	
	int board[XSIZE][YSIZE];
	int score, level, len, direction, px, py;
	
	public:
	Sclange (Adafruit_ST7789 &tftP): tft(tftP), canvas(232, 128), canvas2(280, 38), canvas3(280, 38)/*, canvasBack(280, 240)*/{
		
	}
	
	// Matthias Busse 5.2014
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
	
	void dtob(){
		for (int i = 0; i < 256; i++) {
			analogWrite(26, i);
			delay(3);
		}
	}
	
	void btod(){
		for (int i = 0; i < 256; i++) {
			analogWrite(26, 255-i);
			delay(3);
		}
	}
	
	void drawCentreString(const char *buf, int x, int y)
	{
		int16_t x1, y1;
		uint16_t w, h;
		tft.getTextBounds(buf, 0, 0, &x1, &y1, &w, &h); // get text bounds
		tft.setCursor(x - w / 2, y + h / 2); // set cursor to center
		tft.print(buf);
	}
	
	void drawCentreStringCanvas(const char *buf, int x, int y)
	{
		int16_t x1, y1;
		uint16_t w, h;
		canvas.getTextBounds(buf, 0, 0, &x1, &y1, &w, &h); // get text bounds
		canvas.setCursor(x - w / 2, y + h / 2); // set cursor to center
		canvas.print(buf);
	}
	
	void lostSound()
	{
		for (int i = 0; i < 40; i++)
		{
			digitalWrite(SPEAKER, HIGH);
			delayMicroseconds(2500);
			digitalWrite(SPEAKER, LOW);
			delayMicroseconds(2500);
		}
		
		for (int i = 0; i < 70; i++)
		{
			digitalWrite(SPEAKER, HIGH);
			delayMicroseconds(3500);
			digitalWrite(SPEAKER, LOW);
			delayMicroseconds(3500);
		}
	}
	
	void winSound()
	{
		
		for (int i = 0; i < 40; i++)
		{
			digitalWrite(SPEAKER, HIGH);
			delayMicroseconds(3500);
			digitalWrite(SPEAKER, LOW);
			delayMicroseconds(3500);
		}
		
		for (int i = 0; i < 30; i++)
		{
			digitalWrite(SPEAKER, HIGH);
			delayMicroseconds(2500);
			digitalWrite(SPEAKER, LOW);
			delayMicroseconds(2500);
		}
	}
	
	
	void putPlum()
	{
		if(millis() <= plumtime)return;
		
		int rx = random(0, 15);
		int ry = random(0, 28);
		
		while (board[rx][ry] != 0)
		{
			rx = random(0, 15);
			ry = random(0, 28);
		}
		board[rx][ry] = -2;
		plumtime = millis()+2000;
	}
	
	void putApple()
	{
		int rx = random(0, 15);
		int ry = random(0, 28);
		
		while (board[rx][ry] != 0)
		{
			rx = random(0, 15);
			ry = random(0, 28);
		}
		board[rx][ry] = -1;
	}
	
	void initializeS()
	{
		score = 0;
		level = 0;
		len = 3;
		px = 7;
		py = 14;
		
		for (int ix = 0; ix < 15; ix++)
		{
			for (int iy = 0; iy < 28; iy++)
			{
				board[ix][iy] = 0;
			}
		}
	}
	
	void writeScoreS(const char* msg, int score) {
		
		canvas3.drawRGBBitmap(0, 0, canvas2.getBuffer(), canvas2.width(), canvas2.height());
		canvas3.setCursor(25, 25);
		canvas3.print(msg);
		if (score > -1)
		canvas3.print(score);
		tft.drawRGBBitmap(0, 0, canvas3.getBuffer(), canvas3.width(), canvas3.height());
	}
	
	
	int getXY(int x, int y) {
		if (x < 0 || x > XSIZE - 1 || y < 0 || y > YSIZE - 1) {
			return 0;
			} else {
			return board[x][y];
		}
	}
	
	void drawBow(int x, int y, char dir) {
		if (dir == 0) {
			for (int i = 0; i < 8; i++) {
				for (int j = 0; j < 8; j++) {
					if (bow[i][j]) {
						canvas.drawPixel(x + i, y + j, SNACOL); //?
					}
				}
			}
		}
		if (dir == 1) {
			for (int i = 0; i < 8; i++) {
				for (int j = 0; j < 8; j++) {
					if (bow[i][j]) {
						canvas.drawPixel(x + (7 - i), y + j, SNACOL); //?
					}
				}
			}
		}
		if (dir == 2) {
			for (int i = 0; i < 8; i++) {
				for (int j = 0; j < 8; j++) {
					if (bow[i][j]) {
						canvas.drawPixel(x + i, y + (7 - j), SNACOL); //?
					}
				}
			}
		}
		if (dir == 3) {
			for (int i = 0; i < 8; i++) {
				for (int j = 0; j < 8; j++) {
					if (bow[i][j]) {
						canvas.drawPixel(x + (7 - i), y + (7 - j), SNACOL); //?
					}
				}
			}
		}
	}
	
	void drawHead(int x, int y, char dir) {
		if (dir == LEFT) {
			for (int i = 0; i < 8; i++) {
				for (int j = 0; j < 8; j++) {
					if (head[i][j] > 0) {
						canvas.drawPixel(x + j, y + i, head[i][j]); //?
					}
				}
			}
		}
		if (dir == UP) {
			for (int i = 0; i < 8; i++) {
				for (int j = 0; j < 8; j++) {
					if (head[i][j] > 0) {
						canvas.drawPixel(x + (7 - i), y + j, head[i][j]); //?
					}
				}
			}
		}
		if (dir == DOWN) {
			for (int i = 0; i < 8; i++) {
				for (int j = 0; j < 8; j++) {
					if (head[i][j] > 0) {
						canvas.drawPixel(x + i, y + (7 - j), head[i][j]); //?
					}
				}
			}
		}
		if (dir == RIGHT) {
			for (int i = 0; i < 8; i++) {
				for (int j = 0; j < 8; j++) {
					if (head[i][j] > 0) {
						canvas.drawPixel(x + (7 - j), y + (7 - i), head[i][j]); //?
					}
				}
			}
		}
	}
	
	
	void drawObject(int x, int y, uint16_t object [8][8], char dir) {
		if (dir == LEFT) {
			for (int i = 0; i < 8; i++) {
				for (int j = 0; j < 8; j++) {
					if (object[i][j] > 0) {
						canvas.drawPixel(x + j, y + i, object[i][j]); //?
					}
				}
			}
		}
		if (dir == UP) {
			for (int i = 0; i < 8; i++) {
				for (int j = 0; j < 8; j++) {
					if (object[i][j] > 0) {
						canvas.drawPixel(x + (7 - i), y + j, object[i][j]); //?
					}
				}
			}
		}
		if (dir == DOWN) {
			for (int i = 0; i < 8; i++) {
				for (int j = 0; j < 8; j++) {
					if (object[i][j] > 0) {
						canvas.drawPixel(x + i, y + (7 - j), object[i][j]); //?
					}
				}
			}
		}
		if (dir == RIGHT) {
			for (int i = 0; i < 8; i++) {
				for (int j = 0; j < 8; j++) {
					if (object[i][j] > 0) {
						canvas.drawPixel(x + (7 - j), y + (7 - i), object[i][j]); //?
					}
				}
			}
		}
	}
	
	void drawTail(int x, int y, char dir) {
		if (dir == LEFT) {
			for (int i = 0; i < 8; i++) {
				for (int j = 0; j < 8; j++) {
					if (tail[i][j]) {
						canvas.drawPixel(x + j, y + i, SNACOL); //?
					}
				}
			}
		}
		if (dir == UP) {
			for (int i = 0; i < 8; i++) {
				for (int j = 0; j < 8; j++) {
					if (tail[i][j]) {
						canvas.drawPixel(x + (7 - i), y + j, SNACOL); //?
					}
				}
			}
		}
		if (dir == DOWN) {
			for (int i = 0; i < 8; i++) {
				for (int j = 0; j < 8; j++) {
					if (tail[i][j]) {
						canvas.drawPixel(x + i, y + (7 - j), SNACOL); //?
					}
				}
			}
		}
		if (dir == RIGHT) {
			for (int i = 0; i < 8; i++) {
				for (int j = 0; j < 8; j++) {
					if (tail[i][j]) {
						canvas.drawPixel(x + (7 - j), y + (7 - i), SNACOL); //?
					}
				}
			}
		}
	}
	
	void drawSegment(int x, int y, int v) {
		
		if ((x + y) % 2 == 0) {
			canvas.fillRect(x * 8 + 4, y * 8 + 4, 8, 8, BRIGHTTILE);
			} else {
			canvas.fillRect(x * 8 + 4, y * 8 + 4, 8, 8, DARKTILE);
		}
		
		if ((getXY(y - 1, x) == v + 1 && getXY(y + 1, x) == v - 1) || (getXY(y + 1, x) == v + 1 && getXY(y - 1, x) == v - 1)) {
			if (v == 1) {
				if (getXY(y - 1, x) == v + 1)
				drawTail(x * 8 + 4, y * 8 + 4, 4);
				else
				drawTail(x * 8 + 4, y * 8 + 4, 3);
			} else
			canvas.fillRect(x * 8 + 6, y * 8 + 4, 4, 8, SNACOL);
			return;
		}
		
		if ((getXY(y, x - 1) == v + 1 && getXY(y, x + 1) == v - 1) || (getXY(y, x + 1) == v + 1 && getXY(y, x - 1) == v - 1)) {
			if (v == 1) {
				if (getXY(y, x - 1) == v + 1)
				drawTail(x * 8 + 4, y * 8 + 4, 1);
				else
				drawTail(x * 8 + 4, y * 8 + 4, 2);
			} else
			canvas.fillRect(x * 8 + 4, y * 8 + 6, 8, 4, SNACOL);
			return;
		}
		
		if ((getXY(y, x - 1) == v + 1 && getXY(y - 1, x) == v - 1) || (getXY(y - 1, x) == v + 1 && getXY(y, x - 1) == v - 1)) {
			drawBow(x * 8 + 4, y * 8 + 4, 3);
			return;
		}
		
		if ((getXY(y, x - 1) == v + 1 && getXY(y + 1, x) == v - 1) || (getXY(y + 1, x) == v + 1 && getXY(y, x - 1) == v - 1)) {
			drawBow(x * 8 + 4, y * 8 + 4, 1);
			return;
		}
		
		if ((getXY(y, x + 1) == v + 1 && getXY(y - 1, x) == v - 1) || (getXY(y - 1, x) == v + 1 && getXY(y, x + 1) == v - 1)) {
			drawBow(x * 8 + 4, y * 8 + 4, 2);
			return;
		}
		
		if ((getXY(y, x + 1) == v + 1 && getXY(y + 1, x) == v - 1) || (getXY(y + 1, x) == v + 1 && getXY(y, x + 1) == v - 1)) {
			drawBow(x * 8 + 4, y * 8 + 4, 0);
			return;
		}
		
		if (getXY(y, x - 1) == v + 1)
		drawTail(x * 8 + 4, y * 8 + 4, 1);
		if (getXY(y, x + 1) == v + 1)
		drawTail(x * 8 + 4, y * 8 + 4, 2);
		if (getXY(y + 1, x) == v + 1)
		drawTail(x * 8 + 4, y * 8 + 4, 3);
		if (getXY(y - 1, x) == v + 1)
		drawTail(x * 8 + 4, y * 8 + 4, 4);
	}
	
	void drawChessboard() {
		for (int ix = 0; ix < 15; ix++)
		{
			for (int iy = 0; iy < 28; iy++)
			{
				
				if ((ix + iy) % 2 == 0) {
					canvas.fillRect(iy * 8 + 4, ix * 8 + 4, 8, 8, BRIGHTTILE);
					} else {
					canvas.fillRect(iy * 8 + 4, ix * 8 + 4, 8, 8, DARKTILE);
				}
			}
		}
	}
	
	void drawS()
	{
		for (int ix = 0; ix < 15; ix++)
		{
			for (int iy = 0; iy < 28; iy++)
			{
				if (board[ix][iy] > 0)
				{
					if (board[ix][iy] == len)
					{
						drawObject(iy * 8 + 4, ix * 8 + 4, head, direction);
						} else {
						drawSegment(iy, ix, board[ix][iy]);
					}
					
					} else {
					if ((ix + iy) % 2 == 0) {
						canvas.fillRect(iy * 8 + 4, ix * 8 + 4, 8, 8, BRIGHTTILE);
						} else {
						canvas.fillRect(iy * 8 + 4, ix * 8 + 4, 8, 8, DARKTILE);
					}
					if (board[ix][iy] == -1) {
						drawObject(iy * 8 + 4, ix * 8 + 4, apple, 3);
					}
					if (board[ix][iy] == -2) {
						drawObject(iy * 8 + 4, ix * 8 + 4, plum, 3);
						if(millis() > plumtime){
							board[ix][iy]=0;
						}
					}
				}
			}
		}
		
		tft.drawRGBBitmap(24, 65, canvas.getBuffer(), canvas.width(), canvas.height());
	}
	
	bool crash(int dir)
	{
		if (dir == UP && (py == 27 || board[px][py + 1] > 0))
		{
			return true;
		}
		if (dir == DOWN && (py == 0 || board[px][py - 1] > 0))
		{
			return true;
		}
		
		if (dir == LEFT && (px == 0 || board[px - 1][py] > 0))
		{
			return true;
		}
		
		if (dir == RIGHT && (px == 14 || board[px + 1][py] > 0))
		{
			return true;
		}
		
		return false;
	}
	
	void stepS()
	{
		for (int ix = 0; ix < 15; ix++)
		{
			for (int iy = 0; iy < 28; iy++)
			{
				if (board[ix][iy] > 0)
				{
					board[ix][iy] = board[ix][iy] - 1;
				}
			}
		}
	}
	
	void countdown()
	{
		
		for (int i = 3; i > 0; i--)
		{
			drawChessboard();
			
			drawCentreStringCanvas("GET READY!", 116, 40);
			
			writeScoreS("Score: ", 0);
			
			switch (i)
			{
				
				case 1:
				digitalWrite(LEDG, HIGH);
				digitalWrite(LEDY, LOW);
				drawCentreStringCanvas("1", 116, 80);
				break;
				case 2:
				digitalWrite(LEDR, LOW);
				digitalWrite(LEDY, HIGH);
				drawCentreStringCanvas("2", 116, 80);
				break;
				case 3:
				digitalWrite(LEDR, HIGH);
				drawCentreStringCanvas("3", 116, 80);
				break;
				default:
				break;
			}
			tft.drawRGBBitmap(24, 65, canvas.getBuffer(), canvas.width(), canvas.height());
			if(dispoff){
			dtob();
			dispoff = false;
			} else {
			delay(1000);
			}
		}
		
		digitalWrite(LEDG, LOW);
		
		for (int i = 0; i < 8; i++)
		{
			drawChessboard();
			
			if (i % 2 == 1)
			{
				drawCentreStringCanvas("GO!!!", 116, 60);
				
				digitalWrite(LEDR, HIGH);
				digitalWrite(LEDG, HIGH);
				digitalWrite(LEDY, HIGH);
				//delay(25);
				tft.drawRGBBitmap(24, 65, canvas.getBuffer(), canvas.width(), canvas.height());
				digitalWrite(LEDR, LOW);
				digitalWrite(LEDG, LOW);
				digitalWrite(LEDY, LOW);
				delay(25);
			}
			else
			{
				tft.drawRGBBitmap(24, 65, canvas.getBuffer(), canvas.width(), canvas.height());
				delay(50);
				
			}
		}
		digitalWrite(LEDR, LOW);
		digitalWrite(LEDG, LOW);
		digitalWrite(LEDY, LOW);
	}
	
	void playS() {
		countdown();
		writeScoreS("Score: ", 0);
		uint32_t lastTime = 0;
		int lastDir = 0;
		direction = random(1, 5);
		addScore = 0;
		putApple();
		
		while (true)
		{
			while (millis() < lastTime + 55-(len/4))//60
			{
				if (digitalRead(KEY_DOWN) == LOW && lastDir != LEFT)
				{
					direction = RIGHT;
				}
				if (digitalRead(KEY_UP) == LOW && lastDir != RIGHT)
				{
					direction = LEFT;
				}
				if (digitalRead(KEY_LEFT) == LOW && lastDir != UP)
				{
					direction = DOWN;
				}
				if (digitalRead(KEY_RIGHT) == LOW && lastDir != DOWN)
				{
					direction = UP;
				}
			}
			
			lastDir = direction;
			if(random(0, 500)==1)putPlum();
			if (!crash(direction))
			{
				if (direction == LEFT)
				px--;
				if (direction == RIGHT)
				px++;
				if (direction == DOWN)
				py--;
				if (direction == UP)
				py++;
			}
			else
			{
				if ((len - 3)+addScore > eepromReadInt(2))
				{
					eepromWriteInt(2, (len - 3)+addScore);
					writeScoreS("NEW HISCORE: ", (len - 3)+addScore);
					delay(10);
					winSound();
				}
				else
				{
					writeScoreS("FINAL SCORE: ", (len - 3)+addScore);
					delay(10);
					lostSound();
				}
				while (digitalRead(KEY_CENTER) == HIGH && digitalRead(KEY_A) == HIGH && digitalRead(KEY_B) == HIGH)
				{
				}
				break;
			}
			
			if (board[px][py] == -1)
			{
				tick();
				putApple();
				len++;
				writeScoreS("Score: ", (len - 3)+addScore);
				for (int x = 0; x < 15; x++)
				{
					for (int y = 0; y < 28; y++)
					{
						if (board[x][y] > 0)board[x][y]++;
					}
				}
			}
			
			if (board[px][py] == -2)
			{
				tick();
				len++;
				addScore+=4;
				writeScoreS("Score: ", (len - 3)+addScore);
				for (int x = 0; x < 15; x++)
				{
					for (int y = 0; y < 28; y++)
					{
						if (board[x][y] > 0)board[x][y]++;
					}
				}
			}
			
			board[px][py] = len;
			
			drawS();
			stepS();
			
			lastTime = millis();
		}
	}
	
	void tick() {
		digitalWrite(LEDR, HIGH);
		for (int i = 0; i < 50; i++)
		{
			digitalWrite(SPEAKER, HIGH);
			delayMicroseconds(100);
			digitalWrite(SPEAKER, LOW);
			delayMicroseconds(100);
			if (i == 17) {
				digitalWrite(LEDR, LOW);
				digitalWrite(LEDY, HIGH);
				} else if (i == 35) {
				digitalWrite(LEDY, LOW);
				digitalWrite(LEDG, HIGH);
			}
		}
		digitalWrite(LEDG, LOW);
	}
	
	void run() {
		//set_sys_clock_khz(160000, true);
		//tft.init(240, 280);
		//tft.setSPISpeed(125000000);
		
		tft.setRotation(1);
		
		canvas3.setFont(&FreeSansBold12pt7b);
		tft.setFont(&FreeSansBold12pt7b);
		canvas.setFont(&FreeSansBold12pt7b);
		canvas3.setTextColor(ST77XX_BLACK);
		tft.setTextColor(ST77XX_BLACK);
		canvas.setTextColor(ST77XX_BLACK);
		
		canvas.fillScreen(BORDERCOL);
		tft.fillScreen(ST77XX_BLACK);
		
		//pinMode(26, OUTPUT);
		//analogWrite(26, 0);
		
		Serial.begin(9600);
		
		
		//canvasBack.drawRGBBitmap(0, 0, backS, 280, 240);
		//canvasBack.drawRGBBitmap(21, 52, title, 238, 135);
		//tft.drawRGBBitmap(0, 0, canvasBack.getBuffer(), canvasBack.width(), canvasBack.height());
		//canvasBack.drawRGBBitmap(0, 0, backS, 280, 240);
		
		tft.drawRGBBitmap(0, 0, backS, 280, 240);
		tft.drawRGBBitmap(21, 52, title, 238, 135);
		//tft.drawRGBBitmap(0, 0, canvasBack.getBuffer(), canvasBack.width(), canvasBack.height());
		//canvasBack.drawRGBBitmap(0, 0, backS, 280, 240);
		
		drawCentreString("HISCORE", 140, 108);
		char cstr[16];
		itoa(eepromReadInt(2), cstr, 10);
		drawCentreString(cstr, 140, 138);
		
		canvas2.drawRGBBitmap(0, 0, topS, 280, 51);
		
		int16_t  x1, y1;
		uint16_t w, h;
		
		
		
		unsigned long int last;
		last = millis();
		
		dtob();
		
		while ((digitalRead(KEY_CENTER) == HIGH && digitalRead(KEY_A) == HIGH && digitalRead(KEY_B) == HIGH) || millis() < last + 200) {
			delay(25);
		}
		
		btod();
		
		//drawChessboard();
		//canvasBack.drawRGBBitmap(24, 65, canvas.getBuffer(), canvas.width(), canvas.height());
		tft.drawRGBBitmap(24, 65, canvas.getBuffer(), canvas.width(), canvas.height());
		tft.drawRGBBitmap(0, 0, backS, 280, 240);
		//tft.drawRGBBitmap(0, 0, canvasBack.getBuffer(), canvasBack.width(), canvasBack.height());
		writeScoreS("Score: ", 0);
		while (true)
		{
			drawChessboard();
			initializeS();
			playS();
		}
	}
	
	
	
	
};