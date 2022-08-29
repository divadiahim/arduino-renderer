#include <Arduino.h>
#include "mouse.h"
#include "engine_fixed.h"
#include "driver.h"
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
    initialize();
    uint8_t *buffer = (uint8_t *)malloc(sizeof(mouse));
    memcpy_P(buffer, mouse, sizeof(mouse));
    mat4_fixed new_proj = _fstartE();
   // mat4 new_proj_ = startE();
    MouseData mouse_proj;
    //togle inverse display mode
    // PORTD = PORTD & ~(1 << LCD_DC);
    // lcd_write(0x0D);
    // PORTD = PORTD | (1 << LCD_DC);

    while (true)
    {
        memset(framebuf, 0, sizeof(framebuf)); // clear
        mouse_proj = move_mouse(40, buffer);
        draw_cube_fixed(&new_proj,&mouse_proj);
       //draw trinagle
     // drawTriangle(28,37,55,10,55,37);
   //  drawTriangle(28,37,28,10,55,10);
       // plot_line(28,37,55,10);
        fps(5);
        update();
    }
}
