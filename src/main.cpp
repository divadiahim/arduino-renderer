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
Vector2 line = {10, 8};

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
void drawpixel(Vector2 *poz)
{
    PORTD = PORTD & ~(1 << LCD_DC);
    byte y_ = poz->y / 8;
    byte y_pixel = poz->y - y_ * 8;
    byte yy = 0x40 | y_;
    lcd_write(yy);
    PORTD = PORTD | (1 << LCD_DC);
    for (uint8_t i = 0; i < poz->x; i++)
    {
        lcd_write(0x00);
    }
    lcd_write(0x01 << y_pixel);
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
    lcd_write(0xB9); // Set LCD Vop (Contrast).
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
    drawpixel(&line);
    // lcd_write(0x7F);
    // lcd_write(0x05);
    // lcd_write(0x07);
}
