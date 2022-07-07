// #include "engine.h"
#include "engine_fixed.h"
#include "arrays.h"
#include "mouse.h"
#define LCD_CE 6
#define LCD_RESET 7
#define LCD_DC 5
#define LCD_DIN 4
#define LCD_CLK 3
#define LCD_C LOW
#define LCD_D HIGH

extern uint8_t framebuf[6][84];

void lcd_write(byte data);
void clear_ram();
void clear();
void put_pixel(uint8_t x, uint8_t y);
void update();
void plot_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
void print_digit(uint8_t x, uint8_t y, uint8_t number);
void print_num(uint8_t x, uint8_t y, uint32_t number);
void fps(const uint8_t seconds);
void drawTriangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t x3, uint8_t y3);
MouseData move_mouse(uint16_t ms, uint8_t *buffer);