#include "Arduino.h"
#define INTELLI_MOUSE 3
#define SCALING_1_TO_1 0xE6
#define RESOLUTION_8_COUNTS_PER_MM 3

enum Commands
{
    SET_RESOLUTION = 0xE8,
    REQUEST_DATA = 0xEB,
    SET_REMOTE_MODE = 0xF0,
    GET_DEVICE_ID = 0xF2,
    SET_SAMPLE_RATE = 0xF3,
    RESET = 0xFF,
};
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

int _clockPin = 9;
int _dataPin = 10;
bool _supportsIntelliMouseExtensions = false ;

void waitForClockState(int expectedState)
{
    while (digitalRead(_clockPin) != expectedState)
        ;
}

void high(uint8_t pin)
{
    pinMode(pin, INPUT);
    digitalWrite(pin, HIGH);
}

void low(uint8_t pin)
{
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
}

void writeBit(bool bit)
{
    if (bit == HIGH)
    {
        high(_dataPin);
    }
    else
    {
        low(_dataPin);
    }

    waitForClockState(HIGH);
    waitForClockState(LOW);
}

void writeByte(uint8_t data)
{
    bool parityBit = 1;

    high(_dataPin);
    high(_clockPin);
    delayMicroseconds(300);
    low(_clockPin);
    delayMicroseconds(300);
    low(_dataPin);
    delayMicroseconds(10);

    // start bit
    high(_clockPin);

    waitForClockState(LOW);

    // data
    for (byte i = 0; i < 8; i++)
    {
        bool dataBit = bitRead(data, i);
        writeBit(dataBit);
        parityBit = parityBit ^ dataBit;
    }

    // parity bit
    writeBit(parityBit);

    // stop bit
    high(_dataPin);
    delayMicroseconds(50);
    waitForClockState(LOW);

    // wait for mouse to switch modes
    while ((digitalRead(_clockPin) == LOW) || (digitalRead(_dataPin) == LOW))
        ;

    // put a hold on the incoming data
    low(_clockPin);
}

bool readBit()
{
    waitForClockState(LOW);
    bool bit = digitalRead(_dataPin);
    waitForClockState(HIGH);
    return bit;
}

char readByte()
{
    char data = 0;

    high(_clockPin);
    high(_dataPin);
    delayMicroseconds(50);
    waitForClockState(LOW);
    delayMicroseconds(5);

    // consume the start bit
    waitForClockState(HIGH);

    // consume 8 bits of data
    for (byte i = 0; i < 8; i++)
    {
        bitWrite(data, i, readBit());
    }

    // consume parity bit (ignored)
    readBit();

    // consume stop bit
    readBit();

    // put a hold on the incoming data
    low(_clockPin);

    return data;
}

void writeAndReadAck(uint8_t data)
{
    writeByte((char)data);
    readByte();
}


void setSampleRate(uint16_t rate)
{
    writeAndReadAck(SET_SAMPLE_RATE);
    writeAndReadAck(rate);
}

void reset()
{
    writeAndReadAck(RESET);
    readByte(); // self-test status
    readByte(); // mouse ID
}

char getDeviceId()
{
    writeAndReadAck(GET_DEVICE_ID);
    return readByte();
}


void checkIntelliMouseExtensions()
{
    // IntelliMouse detection sequence
    setSampleRate(200);
    setSampleRate(100);
    setSampleRate(80);

    char deviceId = getDeviceId();
    _supportsIntelliMouseExtensions = (deviceId == INTELLI_MOUSE);
}

void setScaling(uint8_t scaling)
{
    writeAndReadAck(scaling);
}

void setRemoteMode()
{
    writeAndReadAck(SET_REMOTE_MODE);
}

void setResolution(uint8_t resolution)
{
    writeAndReadAck(SET_RESOLUTION);
    writeAndReadAck(resolution);
}

void initialize()
{
    high(_clockPin);
    high(_dataPin);
    reset();
    checkIntelliMouseExtensions();
    setResolution(RESOLUTION_8_COUNTS_PER_MM);
    setScaling(SCALING_1_TO_1);
    setSampleRate(40);
    setRemoteMode();
    delayMicroseconds(100);
}

MouseData readData()
{
    MouseData data;

    writeAndReadAck(REQUEST_DATA);
    data.status = readByte();
    data.position.x = readByte();
    data.position.y = readByte();

    if (_supportsIntelliMouseExtensions)
    {
        data.wheel = readByte();
    }

    return data;
};
