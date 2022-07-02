#include <Arduino.h>
#include "mouse.h"
#include "engine.h"
#include "driver.h"

void move_mouse(uint16_t ms, uint8_t *buffer)
{
    static unsigned long lMillis;
    static MouseData data;
    static bool halt;

    static int x = 0;
    static int y = -3;
    static byte y_ = y / 8;
    byte y_pixel = y - y_ * 8;
    // It is best if we declare millis() only once
    byte px_overflow = 0x00;
    if ((data.status == 0x0C) | halt == true)
    {
        halt = true;
        ms *= 100;
    }
    if (data.status == 0x09)
    {
        halt = false;
        ms /= 100;
    }
    unsigned long now = millis();
    if (now - lMillis >= ms)
    {
        // Serial.println(ms);
        data = readData();
        lMillis = now;
        x += data.position.x;
        y -= data.position.y;
        if (x > 75)
            x = 76;
        if (x < 0)
            x = 0;
        if (y > 32)
            y = 32;
        if (y < -2)
            y = -2;

        y_pixel = y - y_ * 8;
        y_ = y / 8;

        // Serial.println(data.status, BIN);
        //  Serial.print("\tx=");
        //  Serial.print(data.position.x);
        //  Serial.print("\ty=");
        //  Serial.print(data.position.y);
        //  Serial.print("\twheel=");
        //  Serial.print(data.wheel);
        //  Serial.println();
        //  Serial.println(x);
        //  Serial.println(y);

        /////////////////////////////////
    }
    if (!halt)
    {
        for (byte i = 0; i < sizeof(mouse); i++)
        {

            framebuf[y_][x + i] |= (*(buffer + i) << y_pixel);
            px_overflow = (*(buffer + i) & 0xFF << 8 - y_pixel);
            framebuf[y_ + 1][x + i] |= px_overflow >> 8 - y_pixel;
        }
    }
}

void init_lcd()
{
    DDRD = 0xF8; // make pins outputs
    PORTD = PORTD & ~(1 << LCD_RESET);
    PORTD = PORTD | (1 << LCD_RESET);
    PORTD = PORTD & ~(1 << LCD_DC);
    PORTD = PORTD & ~(1 << LCD_CE);
    lcd_write(0x21); // LCD Extended Commands.
    lcd_write(0xBF); // Set LCD Vop (Contrast).
    lcd_write(0x04); // Set Temp coefficent.
    lcd_write(0x14); // LCD bias mode 1:48.
    lcd_write(0x20); // LCD Basic Commands
    lcd_write(0x0C); // LCD in normal mode.
    PORTD = PORTD | (1 << LCD_DC);
    clear_ram();
}
int main(void)
{
    init();
    init_lcd();
    Serial.begin(9600);
    uint8_t i = 0;
    bool ok = 0;
    Serial.begin(9600);
    initialize();

    // update();
    uint8_t *buffer = (uint8_t *)malloc(sizeof(mouse));
    memcpy_P(buffer, mouse, sizeof(mouse));
    mat4 new_proj=_startE();


    while (true)
    {
        //     if (i < 83 && ok == 0)
        //     {
        //         i++;
        //     }
        //     if (i == 83 && ok == 0)
        //     {
        //         ok = 1;
        //     }
        //     if (i <= 83 && ok == 1)
        //     {
        //         i--;
        //     }
        //     if (i == 1 && ok == 1)
        //     {
        //         ok = 0;
        //     }

        // plot_line(83 - i, 1, i, 30);
        //drawTriangle(30, 0, 0, 30, 60, 30);
        draw_cube(&new_proj);
        move_mouse(20, buffer);
        fps(5);
        update();
        // delay(1000);
        clear();
     
    }
}
