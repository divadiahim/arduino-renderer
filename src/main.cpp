#include <Arduino.h>
#define LCD_CE 6
#define LCD_RESET 7
#define LCD_DC 5
#define LCD_DIN 4
#define LCD_CLK 3
#define LCD_C LOW
#define LCD_D HIGH
struct Vector2
{
    uint16_t x, y;
};
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
        // shiftOut(LCD_DIN, LCD_CLK, 1, 0x00);
        lcd_write(0x00);
    }
}
void clear()
{
      for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 84; j++)
        {
            framebuf[i][j] = 0;
        }
    }
}
void drawpixel(uint8_t x, uint8_t y) // this is using the command mode | chnges the memeory adress
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
void plot_line(int x0, int y0, int x1, int y1)
{
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2; /* error value e_xy */

    for (;;)
    { /* loop */
        put_pixel(x0, y0);
        if (x0 == x1 && y0 == y1)
            break;
        e2 = 2 * err;
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
    update();
}

void init_lcd()
{
    pinMode(LED_BUILTIN, OUTPUT);
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
    digitalWrite(LCD_DC, HIGH);
    clear_ram();
}
int main(void)
{
    init();
    init_lcd();
    Vector2 line{3, 15};
    uint8_t i = 0;
    bool ok = 0;
    while (true)
    {
        if (i < 83 && ok == 0)
        {
            i++;
        }
        if (i == 83 && ok == 0)
        {
            ok = 1;
        }
        if (i <= 83 && ok == 1)
        {
            i--;
        }
        if (i == 1 && ok == 1)
        {
            ok = 0;
        }

        //plot_line(i, 2, i, 30);;
       // put_pixel(5,i);
        // bhm_line(i,0,i,30);
        
        plot_line(1,1,i,30);
        delay(50);
        clear();
        // clear_ram();
    }

    // lcd_write(0x7F);
    // lcd_write(0x05);
    // lcd_write(0x07);
}
