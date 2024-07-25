#include <Adafruit_GFX.h>
#include "airstrike16pt7b.h"

class StarIntro {
	private:
    Adafruit_ST7789 tft;
	GFXcanvas16 canvas;
	int gameID = 1;
	uint8_t red = 250, green = 0, blue = 0;
	
	public:
    StarIntro (Adafruit_ST7789 &tftP): tft(tftP), canvas(240, 280) {
	}
	
	uint16_t convertRGBtoRGB565(uint8_t red, uint8_t green, uint8_t blue) {
		uint16_t r = (red >> 3) & 0x1F;
		uint16_t g = (green >> 2) & 0x3F;
		uint16_t b = (blue >> 3) & 0x1F;
		uint16_t rgb565 = (r << 11) | (g << 5) | b;
		
		return rgb565;
	}
	
	void colorCycle(uint8_t &red, uint8_t &green, uint8_t &blue) {
		if (red == 250 && green < 250 && blue == 0) {
			green+=25;
			} else if (green == 250 && red > 0 && blue == 0) {
			red-=25;
			} else if (green == 250 && blue < 250 && red == 0) {
			blue+=25;
			} else if (blue == 250 && green > 0 && red == 0) {
			green-=25;
			} else if (blue == 250 && red < 250 && green == 0) {
			red+=25;
			} else if (red == 250 && blue > 0 && green == 0) {
			blue-=25;
		}
	}
	
	uint16_t getStringWidth(String str) {
		int16_t x1, y1;
		uint16_t w, h;
		canvas.getTextBounds(str, 0, 0, &x1, &y1, &w, &h);
		return w;
	}
	int starIntro()
	{
		canvas.setFont(&airstrike16pt7b);
		const int numstars = 	500;
		unsigned long lastSelection = millis();
		float stars[numstars][4];
		
		int eescoreS = eepromReadInt(2);
		int eescoreT = eepromReadInt(0);
		int eescoreTM = eepromReadInt(4);
		int eescoreP = eepromReadInt(6);
		
		for (int i = 0; i < numstars; i++)
		{
			stars[i][0] = 67;
			stars[i][1] = 120;
			float angle = (random(3600) / 1800.0) * PI;
			float speed = random(450) + 450;
			stars[i][2] = speed * sin(angle);
			stars[i][3] = speed * cos(angle);
		}
		
		while (digitalRead(KEY_CENTER) == HIGH && digitalRead(KEY_A) == HIGH && digitalRead(KEY_B) == HIGH)
		{
			canvas.fillScreen(ST77XX_BLACK);
			for (int i = 0; i < numstars; i++)
			{
				
				canvas.drawPixel(stars[i][1], stars[i][0],0xBD76);
				stars[i][0] += stars[i][2] / 1000;
				stars[i][1] += stars[i][3] / 1000;
				
				stars[i][2] *= 1.1;
				stars[i][3] *= 1.1;
				
				if (stars[i][0] < 0 || stars[i][0] > 280 || stars[i][1] < 0 || stars[i][1] > 240)
				{
					stars[i][0] = 67;
					stars[i][1] = 120;
					float angle = (random(3600) / 1800.0) * PI;
					float speed = random(450) + 450;
					stars[i][2] = speed * sin(angle);
					stars[i][3] = speed * cos(angle);
				}
			}
			
			if (millis() % 6000 < 1500)
			{
				canvas.setCursor(120 - getStringWidth("PICTRIS") / 2, 130);
				canvas.println("PICTRIS");
				canvas.setCursor(120 - getStringWidth("HI-SCORE") / 2, 100);
				canvas.println("HI-SCORE");
				char cstr[16];
				itoa(eescoreT, cstr, 10);
				canvas.setCursor(120 - getStringWidth(cstr) / 2, 160);
				canvas.println(cstr);
			}
			else if (millis() % 6000 < 3000)
			{
				canvas.setCursor(120 - getStringWidth("SCLANGE") / 2, 130);
				canvas.println("SCLANGE");
				canvas.setCursor(120 - getStringWidth("HI-SCORE") / 2, 100);
				canvas.println("HI-SCORE");
				char cstr[16];
				itoa(eescoreS, cstr, 10);
				canvas.setCursor(120 - getStringWidth(cstr) / 2, 160);
				canvas.println(cstr);
			}
			else if (millis() % 6000 < 4500)
			{
				canvas.setCursor(120 - getStringWidth("TABMAN") / 2, 130);
				canvas.println("TABMAN");
				canvas.setCursor(120 - getStringWidth("HI-SCORE") / 2, 100);
				canvas.println("HI-SCORE");
				char cstr[16];
				itoa(eescoreTM, cstr, 10);
				canvas.setCursor(120 - getStringWidth(cstr) / 2, 160);
				canvas.println(cstr);
			}
			else
			{
				canvas.setCursor(120 - getStringWidth("POOPIE") / 2, 130);
				canvas.println("POOPIE");
				canvas.setCursor(120 - getStringWidth("HI-SCORE") / 2, 100);
				canvas.println("HI-SCORE");
				char cstr[16];
				itoa(eescoreP, cstr, 10);
				canvas.setCursor(120 - getStringWidth(cstr) / 2, 160);
				canvas.println(cstr);
			}
			
			if ( millis() % 600 < 150) {
				analogWrite(LEDR, 65);
				analogWrite(LEDY, 0);
				analogWrite(LEDG, 0);
				} else if (millis() % 600 < 300) {
				analogWrite(LEDR, 0);
				analogWrite(LEDY, 15);
				analogWrite(LEDG, 0);
				} else if (millis() % 600 < 450) {
				analogWrite(LEDR, 0);
				analogWrite(LEDY, 0);
				analogWrite(LEDG, 185);
				} else {
				analogWrite(LEDR, 0);
				analogWrite(LEDG, 30);
				analogWrite(LEDG, 0);
			}
			
			if (millis() % 500 < 250)
			{
				canvas.setCursor(120 - getStringWidth("Select") / 2, 216);
				canvas.println("Select");
				if (gameID == 1)
				{
					canvas.setCursor(120 - getStringWidth("<                    >") / 2, 246);
					canvas.println("<                    >");
					canvas.setCursor(120 - getStringWidth("PICTRIS") / 2, 246);
					canvas.setTextColor(ST77XX_BLUE);
					canvas.println("PICTRIS");
					canvas.setTextColor(ST77XX_WHITE);
				}
				else if (gameID == 2)
				{
					canvas.setCursor(120 - getStringWidth("<                    >") / 2, 246);
					canvas.println("<                    >");
					canvas.setCursor(120 - getStringWidth("SCLANGE") / 2, 246);
					canvas.setTextColor(ST77XX_GREEN);
					canvas.println("SCLANGE");
					canvas.setTextColor(ST77XX_WHITE);
				}
				else if (gameID == 3)
				{
					canvas.setCursor(120 - getStringWidth("<                    >") / 2, 246);
					canvas.println("<                    >");
					canvas.setCursor(120 - getStringWidth("TABMAN") / 2, 246);
					canvas.setTextColor(ST77XX_RED);
					canvas.println("TABMAN");
					canvas.setTextColor(ST77XX_WHITE);
				}
				else
				{
					canvas.setCursor(120 - getStringWidth("<                    >") / 2, 246);
					canvas.println("<                    >");
					canvas.setCursor(120 - getStringWidth("POOPIE") / 2, 246);
					canvas.setTextColor(ST77XX_YELLOW);
					canvas.println("POOPIE");
					canvas.setTextColor(ST77XX_WHITE);
				}
				
			}    
			
			
			canvas.setCursor(120 - getStringWidth("PicoPac") / 2, 50);
			colorCycle(red, green, blue);
            uint16_t rgb565 = convertRGBtoRGB565(red, green, blue);
			canvas.setTextColor(rgb565);
			canvas.println("PicoPac");
			canvas.setTextColor(ST77XX_WHITE);
			
			tft.drawRGBBitmap(0, 0, canvas.getBuffer(), canvas.width(), canvas.height());
			
			if (digitalRead(KEY_DOWN) == LOW && millis() > lastSelection + 200)
			{
				gameID = gameID - 1;
				if (gameID == 0)
				gameID = 4;//2
				lastSelection = millis();
				// break;
			}
			else if (digitalRead(KEY_UP) == LOW && millis() > lastSelection + 200)
			{
				gameID = gameID + 1;
				if (gameID == 5)//3
				gameID = 1;
				lastSelection = millis();
				// break;
			}
		}
		delay(10);
		analogWrite(LEDR, 0);
		analogWrite(LEDG, 0);
		analogWrite(LEDY, 0);
		while (digitalRead(KEY_CENTER) == LOW && digitalRead(KEY_A) == LOW && digitalRead(KEY_B) == LOW) {}
		delay(90);
		return gameID;
	}
	
	
};							