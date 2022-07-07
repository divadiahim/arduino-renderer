#include "driver.h"
#include "mouse.h"
uint8_t framebuf[6][84];
void lcd_write(byte data)
{

    byte port_cpy = PORTD;
    port_cpy = port_cpy | (1 << 4);
    for (uint8_t i = 0; i < 8; i++)
    {
        // if ((data & 0x80) == 0x80)
        //     PORTD = PORTD | (1 << LCD_DIN);
        // else
        //     PORTD = PORTD & ~(1 << LCD_DIN);
        PORTD = (port_cpy | ((data & 0x80) >> 3)) ^ ((~((data & 0x80) >> 7) & 0x01) << 4);
        data <<= 1;
        PORTD = PORTD | (1 << LCD_CLK);
        PORTD = PORTD & ~(1 << LCD_CLK);
    }
}
void clear_ram()
{
    for (uint16_t i = 0; i <= 503; i++)
    {
        lcd_write(0x00);
    }
}
void clear()
{
    memset(framebuf, 0, sizeof(framebuf));
}
void drawpixel(uint8_t x, uint8_t y) // this is using the command mode | chnges the memeory adress//legagy broken code
{
    PORTD = PORTD & ~(1 << LCD_DC);
    byte y_ = y / 8;
    byte y_pixel = y - y_ * 8;
    byte yy = 0x40 | y_;
    byte xx = 0x80 | x;
    lcd_write(yy);
    lcd_write(xx);
    PORTD = PORTD | (1 << LCD_DC);
    lcd_write(0x01 << y_pixel);
}
void put_pixel(uint8_t x, uint8_t y)
{
    byte y_ = y / 8;
    byte y_pixel = y - y_ * 8;
    if (x < 84 && y_ < 6 && y_ >= 0)
        framebuf[y_][x] |= (0x01 << y_pixel);
}
void update()
{
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 84; j++)
        {
            lcd_write(framebuf[i][j]);
        }
    }
}
void plot_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
    uint8_t dx = abs(x1 - x0);
    int8_t sx = x0 < x1 ? 1 : -1;
    int16_t dy = -abs(y1 - y0);
    int8_t sy = y0 < y1 ? 1 : -1;
    int16_t err = dx + dy, e2; /* error value e_xy */

    for (;;)
    { /* loop */
        put_pixel(x0, y0);
        if (x0 == x1 && y0 == y1)
            break;
        e2 = 4 * err;
        if (e2 >= dy)
        {
            err += dy;
            x0 += sx;
        } /* e_xy+e_x > 0 */
        if (e2 <= dx)
        {
            err += dx;
            y0 += sy;
        } /* e_xy+e_y < 0 */
    }
}
void print_digit(uint8_t x, uint8_t y, uint8_t number)
{
    uint8_t *buffer = (uint8_t *)malloc(sizeof(nums));
    memcpy_P(buffer, nums, sizeof(nums));
    uint8_t counter = 0, z = 1;
    byte y_ = y / 8;
    for (uint8_t i = 0; i < sizeof(nums); i++)
    {
        if (*(buffer + i) == 0x00)
        {
            counter++;
            z = 0;
        }
        if (counter == number)
        {
            z++;
            framebuf[y_][x + z] |= (*(buffer + i));
        }
    }
    free(buffer);
}
static uint8_t count_ifs(uint32_t n)
{
    if (n < 10)
        return 1;
    if (n < 100)
        return 2;
    if (n < 1000)
        return 3;
    return 0;    
}
void print_num(uint8_t x, uint8_t y, uint32_t number)
{
    uint8_t n = 0;
    n = count_ifs(number);
    for (byte i = 0; i < n; i++)
    {
        print_digit((i * 5) + x, y, (number / (int)pow(10, n - i - 1) % 10));
    }
}
void fps(const uint8_t seconds)
{
    // Create static variables so that the code and variables can
    // all be declared inside a function
    static unsigned long lastMillis;
    static unsigned long frameCount;
    static uint16_t framesPerSecond;

    // It is best if we declare millis() only once
    unsigned long now = millis();
    frameCount++;
    if (now - lastMillis >= seconds * 1000)
    {
        framesPerSecond = frameCount / seconds;
        frameCount = 0;
        lastMillis = now;
    }
    print_num(66, 0, framesPerSecond);
}
void drawTriangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t x3, uint8_t y3)
{
    plot_line(x1, y1, x2, y2);
    plot_line(x1, y1, x3, y3);
    plot_line(x2, y2, x3, y3);
}
MouseData move_mouse(uint16_t ms, uint8_t *buffer)
{
    static unsigned long lMillis;
    static MouseData data;
    static bool halt;
    static int x = 0;
    static int y = -3;
    static byte y_ = y / 8;
    byte y_pixel = y - y_ * 8;
    // It is best if we declare millis() only once
    static byte px_overflow = 0x00;
    if ((data.status == 0x0C) | (halt == true))
    {
        halt = true;
        ms *= 100;
    }
    if (data.status == 0x09)
    {
        halt = false;
        ms /= 100;
    }

    if (!halt)
    {
        for (byte i = 0; i < sizeof(mouse); i++)
        {

            framebuf[y_][x + i] |= (*(buffer + i) << y_pixel);
            px_overflow = (*(buffer + i) & 0xFF << (8 - y_pixel));
            framebuf[y_ + 1][x + i] |= px_overflow >> (8 - y_pixel);
        }
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
        y_ = y / 8;
        y_pixel = y - y_ * 8;
        return data;
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
    }
}
//Never goona give you up
//Never goona let you down
//Never goona run around and desert you
//Never goona make you cry
//Never goona say goodbye
//Never goona tell a lie and hurt you
//Never goona turn you around
//Never goona cut you down