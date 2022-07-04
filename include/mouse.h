#ifndef MOUSE_H_

#define MOUSE_H_

typedef struct
{
    int x, y;
} Position;

typedef struct
{
    int status;
    Position position;
    int wheel;
} MouseData;



extern int _clockPin;
extern int _dataPin;
extern bool _supportsIntelliMouseExtensions;

void initialize();

MouseData readData();


#endif // MOUSE_H_