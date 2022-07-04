#include <Arduino.h>
#include "mouse.h"
#include "engine.h"
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
    uint8_t i = 0;
    bool ok = 0;
    Serial.begin(9600);
    initialize();

    // update();
    uint8_t *buffer = (uint8_t *)malloc(sizeof(mouse));
    memcpy_P(buffer, mouse, sizeof(mouse));
    mat4 new_proj = _startE();
    MouseData mouse_proj;
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
        // drawTriangle(30, 0, 0, 30, 60, 30);
        clear();
        mouse_proj=move_mouse(20, buffer);
        draw_cube(&new_proj,&mouse_proj);
        
        fps(5);
        update();
        // delay(500);
    }
}
