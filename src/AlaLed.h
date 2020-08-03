#ifndef AlaLed_h
#define AlaLed_h

#include "Ala.h"

#define DEBUGPRINTS
#define MAX_PWM_VAL 255
#define MIN_PWM_VAL 0

#pragma pack(push, 1)

struct AlaSeq
{
    uint8_t animation;
    uint16_t speed;
    uint16_t duration;
    // AlaPalette palette;
};

#pragma pack(pop)


#define MAX_B_LEVELS 4

/**
 *  AlaLed can be used to drive a single or multiple output channels to perform animations.
 *  Single leds can be attached to each channel or MOSFETS/relays can be used to drive
 *  heavier loads such as lamps or monochrome led strips.
 */
class AlaLed
{

public:

    AlaLed();


    /**
    * Initializes a LED driven by Arduino PWM pin. It be invoked in the setup() function of the main Arduino sketch.
    */
    void initPWM(byte pin);

    /**
    * Initializes LEDs driven by Arduino PWM pins. It be invoked in the setup() function of the main Arduino sketch.
    */
    void initPWM(int numLeds, byte *pins);

    /**
    * Initializes LEDs driven by a TLC5940 chip. It be invoked in the setup() function of the main Arduino sketch.
    * The TLC5940 must be wired as follows (http://XYXYXYXY)
    * pin  2 > GND
    * pin  3 > Arduino 11
    * pin  4 > Arduino 13
    * pin  5 > Arduino 9
    * pin  6 > Arduino 10 + XXX resistor to +5V
    * pin  7 > GND
    * pin  8 > +5V
    * pin  9 > xxx resistor to GND
    * pin 10 > +5V
    * pin 11 > Arduino 3
    *
    * It can be used also to drive a single led.
    */
    // void initTLC5940(int numLeds, byte *pins);


    /**
    * Sets the maximum brightness level (0-255).
    */
    void setBrightness(uint16_t maxOut);

    /**
    * Sets the maximum refresh rate in Hz (default value is 50 Hz).
    * May be useful to reduce flickering in some cases.
    */
    void setRefreshRate(int refreshRate);

    int getRefreshRate();

    void setAnimation(uint8_t animation, uint32_t speed, bool isSeq=false);
    void setAnimation(AlaSeq animSeq[]);
	void setSpeed(uint32_t speed);
    int getAnimation();

    bool runAnimation();

    // set LED driver
    void setDriver(byte driver = ALA_PWM);


private:

    void setAnimationFunc(uint8_t animation);
    void on();
    void off();
    void skip();
    void blink();
    void blinkAlt();
    void sparkle();
    void sparkle2();
    void strobo();

    void pixelShiftRight();
    void pixelShiftLeft();
    void pixelBounce();
    void pixelSmoothShiftRight();
    void pixelSmoothShiftLeft();
    void pixelSmoothBounce();
    void comet();
    void barShiftRight();
    void barShiftLeft();
    void larsonScanner();
    void larsonScanner2();

    void fadeIn();
    void fadeOut();
    void fadeInOut();
    void glow();
    void flame();

    void fadeTo();
    void setLevel0();
    void setLevel1();
    void setLevel2();
    void setLevel3();


    byte driver; // type of led driver: ALA_PWM, ALA_TLC5940
    byte *pins;  // pins where the leds are attached to
    int16_t *leds;  // array to store leds brightness values
    uint8_t numLeds; // number of leds

    uint16_t animation_step = 0;
    uint8_t britness_level[MAX_B_LEVELS];

    int16_t maxOut;
    uint16_t refreshMillis;
    uint16_t refreshRate;   // current refresh rate

    uint8_t animation;
    //uint8_t animation_last;
    uint32_t speed;
    uint32_t duration;
    AlaSeq *animSeq;
    uint16_t animSeqLen;
    uint32_t animSeqDuration;

    void (AlaLed::*animFunc)();
    uint32_t animStartTime;
    uint32_t animSeqStartTime;
    uint32_t lastRefreshTime;

};

#endif