#include <Adafruit_GFX.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include "image.h"
#include "top.h"
#include "blocks.h"
#include "back.h"

#define LEDR 14
#define LEDY 13
#define LEDG 12
#define KEY_RIGHT 2 //2
#define KEY_DOWN 3 //3
#define KEY_LEFT 4 //4
#define KEY_UP 1 //1
#define KEY_CENTER 0 //0
#define SPEAKER 15

class Pictris {
	private:
	Adafruit_ST7789 tft;
	GFXcanvas16 canvas;
	GFXcanvas16 canvas2;
	GFXcanvas16 canvas3;
	GFXcanvas16 canvas4;
	byte currentcolor;
	byte nextcolor;
	byte fix[10][20];//   grün   rot       blau
	uint16_t color1[3]= {0x054E, 0xF800, 0x001f}; //rand //
	uint16_t color2[3]= {0x07e0, 0xFDD6, 0x249F}; //füllung, heller
	int type, rotation;
	int x, y;
	int nextType;
	int numblocks;
	int score, level;
	int leveltime;
	public:
	Pictris (Adafruit_ST7789 &tftP): tft(tftP), canvas(100, 211), canvas2(240, 38), canvas3(240, 38), canvas4(100, 211){
		
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
	
	void drawCentreStringCanvas3(const char *buf, int x, int y)
	{
		int16_t x1, y1;
		uint16_t w, h;
		canvas3.getTextBounds(buf, 0, 0, &x1, &y1, &w, &h); // get text bounds
		canvas3.setCursor(x - w / 2, y + h / 2); // set cursor to center
		canvas3.print(buf);
	}
	
	void drawCentreStringCanvas(const char *buf, int x, int y)
	{
		int16_t x1, y1;
		uint16_t w, h;
		canvas.getTextBounds(buf, 0, 0, &x1, &y1, &w, &h); // get text bounds
		canvas.setCursor(x - w / 2, y + h / 2); // set cursor to center
		canvas.print(buf);
	}
	
	void writeScore(int score) {
		canvas3.drawRGBBitmap(0, 0, canvas2.getBuffer(), canvas2.width(), canvas2.height());
		char cstr[16];
		itoa(score, cstr, 10);
		drawCentreStringCanvas3(cstr, 120, 18);
		tft.drawRGBBitmap(0, 0, canvas3.getBuffer(), canvas3.width(), canvas3.height());
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
	
	
	void drawBlock(int x, int y, bool filled, byte color)
	{
		y += 2;
		int basex = x * 10 - 1;
		int basey = 30 + (y - 4) * 10;
		canvas.drawRect(basex + 1, basey + 1, 10, 10, color1[color - 1]);
		canvas.drawRect(basex + 2, basey + 2, 8, 8, color1[color - 1]);
		if (filled) {
			canvas.fillRect(basex + 3, basey + 3, 6, 6, color2[color - 1]);
		}
	}
	
	void drawFix()
	{
		for (int i = 0; i < 10; i++)
		{
			for (int j = 4; j < 20; j++)
			{
				if (fix[i][j] > 0)
				{
					drawBlock(i, j, true, fix[i][j]);
				}
			}
		}
	}
	
	void drawTetromino()
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (blocks[type][rotation][j][i] == true)
				{
					drawBlock(i + x, j + y, true, currentcolor);
				}
			}
		}
	}
	
	void drawNext()
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (blocks[nextType][0][j][i] == true)
				{
					drawBlock(i + 6, j - 3, true, nextcolor);
				}
			}
		}
	}
	
	bool fits(int x, int y, int rotation)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (blocks[type][rotation][j][i] == true && (i + x < 0))
				{
					return false;
				}
				
				if (blocks[type][rotation][j][i] == true && (i + x > 9))
				{
					return false;
				}
				
				if (blocks[type][rotation][j][i] == true && (j + y > 19))
				{
					return false;
				}
				
				if (blocks[type][rotation][j][i] == true && fix[i + x][j + y] > 0)
				{
					return false;
				}
			}
		}
		return true;
	}
	
	void rotateTetromino()
	{
		if (fits(x, y, (rotation + 1) % 4))
		{
			rotation = (rotation + 1) % 4;
		}
	}
	
	void newTetromino()
	{
		x = 3;
		y = 0;
		type = nextType;
		currentcolor = nextcolor;
		nextType = rand() % 7;
		nextcolor = 1 + rand() % 3;
		rotation = 0;
	}
	
	bool step()
	{
		if (fits(x, y + 1, rotation))
		{
			y++;
			return true;
		}
		return false;
	}
	
	void moveLeft()
	{
		if (fits(x - 1, y, rotation))
		{
			x--;
		}
	}
	
	void moveRight()
	{
		if (fits(x + 1, y, rotation))
		{
			x++;
		}
	}
	
	void settle()
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (blocks[type][rotation][j][i])
				{
					fix[i + x][j + y] = currentcolor;
				}
			}
		}
	}
	
	bool clean()
	{
		bool cleant = false;
		int n = 0;
		for (int line = 19; line > 0; line--)
		{
			bool complete = true;
			for (int column = 0; column < 10; column++)
			{
				if (fix[column][line] == 0)
				{
					complete = false;
				}
			}
			if (complete)
			{
				cleant = true;
				for (int lineC = line; lineC > 1; lineC--)
				{
					for (int column = 0; column < 10; column++)
					{
						fix[column][lineC] = fix[column][lineC - 1];
					}
				}
				line++;
				n++;
			}
		}
		switch (n)
		{
			case 1:
			score += 40 * (level + 1);
			writeScore(score);
			break;
			case 2:
			score += 100 * (level + 1);
			writeScore(score);
			break;
			case 3:
			score += 300 * (level + 1);
			writeScore(score);
			break;
			case 4:
			score += 1200 * (level + 1);
			writeScore(score);
			break;
			default:
			break;
		}
		if (cleant)
		{
			tick();
		}
		return cleant;
	}
	
	void initialize()
	{
		score = 0;
		level = 0;
		numblocks = 0;
		leveltime = 500;
		nextType = rand() % 7;
		
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 20; j++)
			{
				fix[i][j] = 0;
			}
		}
	}
	
	void drawAll()
	{
		//canvas.fillScreen(0x0000  );
		canvas.drawRGBBitmap(0, 0, canvas4.getBuffer(), 100, 211);
		drawFix();
		drawTetromino();
		drawNext();
		canvas.drawLine(0, 25, 100, 25, ST77XX_BLACK);
		tft.drawRGBBitmap(70, 52, canvas.getBuffer(), canvas.width(), canvas.height());
	}
	
	void playT(void)
	{
		unsigned long last;
		unsigned long lastCTRL;
		unsigned long lastROT;
		bool run;
		bool pressedBefore;
		int steerBefore;
		
		while (true)
		{
			last = millis();
			lastCTRL = millis();
			lastROT = millis();
			run = true;
			pressedBefore = false;
			steerBefore = false;
			
			newTetromino();
			numblocks++;
			while (run)
			{
				
				while (millis() < last + leveltime)
				{
					if (steerBefore > 1)
					{
						if (digitalRead(KEY_UP) == LOW && (millis() > (lastCTRL + leveltime / 4)))
						{
							moveRight();
							lastCTRL = millis();
							drawAll();
							steerBefore++;
						}
						
						if (digitalRead(KEY_DOWN) == LOW && (millis() > (lastCTRL + leveltime / 4)))
						{
							moveLeft();
							lastCTRL = millis();
							drawAll();
							steerBefore++;
						}
					}
					else
					{
						if (digitalRead(KEY_UP) == LOW && (millis() > (lastCTRL + leveltime / 2)))
						{
							moveRight();
							lastCTRL = millis();
							drawAll();
							steerBefore++;
						}
						
						if (digitalRead(KEY_DOWN) == LOW && (millis() > (lastCTRL + leveltime / 2)))
						{
							moveLeft();
							lastCTRL = millis();
							drawAll();
							steerBefore++;
						}
					}
					
					if (!(digitalRead(KEY_UP) == LOW || digitalRead(KEY_DOWN) == LOW))
					{
						steerBefore = LOW;
					}
					
					if (!pressedBefore && digitalRead(KEY_LEFT) == LOW && (millis() > (lastROT + leveltime / 4))) // 4
					{
						rotateTetromino();
						lastROT = millis();
						drawAll();
						pressedBefore = true;
					}
					
					if (digitalRead(KEY_LEFT) == HIGH)
					{
						pressedBefore = false;
						delay(8);
					}
					
					if (digitalRead(KEY_RIGHT) == LOW && (millis() > (lastCTRL + leveltime / 8)))
					{
						lastCTRL = millis();
						last -= 1000;
					}
				}
				if (step())
				{
				}
				else
				{
					settle();
					newTetromino();
					if (!fits(x, y, rotation))
					{ 
				dimCanvas();
					canvas.setCursor(40, 30); //
						if (score > eepromReadInt(0))
						{
							eepromWriteInt(0, score);
							drawCentreStringCanvas("T",20,50);
							drawCentreStringCanvas("O",50,110);
							drawCentreStringCanvas("P",80,170);
						} else {
							//drawCentreStringCanvas("T",20,50);
							//drawCentreStringCanvas("O",50,110);
							//drawCentreStringCanvas("P",80,170);
						}
						
						
						
						tft.drawRGBBitmap(70, 52, canvas.getBuffer(), canvas.width(), canvas.height());
						
						delay(1000);
						while (digitalRead(KEY_CENTER) == HIGH)
						{
						}
						writeScore(0);
						initialize();
					return;
					
					}
					numblocks++;
					if (numblocks >= 15) // 15
					{
					level++;
					leveltime *= 0.8;
					numblocks = 0;
					}
					}
					
					drawAll();
					
					if (clean())
					{
					delay(leveltime / 8);
					drawAll();
					}
					
					last = millis();
					}
					}
					}
					
					void dimCanvas() {
					for (int i = 0; i < canvas.width() + 1; i++) {
					for (int j = 0; j <  canvas.height() + 1; j++) {
					if ((i + j) % 2 == 0) {
					canvas.drawPixel(i, j, 0x8430 );
					}
					}
					}
					}
					
					void run() {
					
					tft.setTextColor(ST77XX_BLACK);
					canvas.setFont(&FreeSansBold18pt7b);
					canvas.setTextColor(ST77XX_BLACK);
					canvas3.setFont(&FreeSansBold18pt7b);
					canvas3.setTextColor(ST77XX_BLACK);
					
					canvas4.drawRGBBitmap(0, 0, back, 100, 211);
					tft.drawRGBBitmap(0, 0, finger, 240, 280);
					tft.setCursor(31, 40); // set cursor to center
					tft.print("Push to play");
					tft.setCursor(31, 53); 
					tft.print("Highscore: ");
					tft.print(eepromReadInt(0));
					for (int i = 0; i < 100; i++) {
					analogWrite(26, i);
					delay(10);
					}
					tft.setFont(&FreeSansBold18pt7b);
					
					canvas2.drawRGBBitmap(0, 0, top, 240, 38);
					currentcolor = 1 + rand() % 3;
					nextcolor = 1 + rand() % 3;
					
					
					unsigned long int last;
					last = millis();
					
					while (digitalRead(KEY_CENTER) == HIGH || millis() < last + 200) {
					if (millis() % 600 > 300) {
					}
					delay(25);
					}
					randomSeed(millis());
					tft.drawRGBBitmap(0, 0, finger, 240, 70);
					writeScore(0);
					while (true)
					{
					
					initialize();
					playT();
					}
					}
					
					};
										