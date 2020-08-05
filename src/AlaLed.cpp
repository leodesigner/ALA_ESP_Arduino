#include "AlaLed.h"

//#include "ExtTlc5940.h"


AlaLed::AlaLed()
{
    maxOut = 255;
    speed = 1000;
    animSeqLen = 0;
    lastRefreshTime = 0;
    refreshMillis = 1000/50;
    britness_level[0] = 0;
    britness_level[1] = 85;
    britness_level[2] = 170;
    britness_level[3] = 255;
    animation_step = 0;
    animation = ALA_STOPSEQ;
}



void AlaLed::initPWM(byte pin)
{
    byte *pins_ = (byte *)malloc(1);
    pins_[0] = pin;

    initPWM(1, pins_);
}


void AlaLed::initPWM(int numLeds, byte *pins)
{
    this->driver = ALA_PWM;
    this->numLeds = numLeds;
    this->pins = pins;
    analogWriteRange(MAX_PWM_VAL);

    for (int x=0; x<numLeds ; x++)
    {
        pinMode(pins[x], OUTPUT);
    }

    // allocate and clear leds array
    leds = (int16_t *)malloc(numLeds * sizeof(int16_t));
    memset(leds, 0, numLeds * sizeof(int16_t));
}

void AlaLed::setDriver(byte driver) 
{
        this->driver = driver;
}

/*
void AlaLed::initTLC5940(int numLeds, byte *pins)
{
    this->driver = ALA_TLC5940;
    this->numLeds = numLeds;
    this->pins = pins;

    // allocate and clear leds array
    leds = (byte *)malloc(numLeds);
    memset(leds, 0, numLeds);

    // call Tlc.init only once
    static bool isTlcInit = false;
    if(!isTlcInit)
    {
        Tlc.init(0);
        isTlcInit=true;
    }
}
*/


void AlaLed::setBrightness(uint16_t maxOut)
{
    this->maxOut = maxOut;
}

void AlaLed::setRefreshRate(int refreshRate)
{
    this->refreshMillis = 1000/refreshRate;
}


void AlaLed::setAnimation(uint8_t animation, uint32_t speed, bool isSeq)
{
    if (this->animation == animation && this->speed == speed) 
    {
        this->animation_step++;
        return;
    }

    this->animation_step = 0;
    this->animation = animation;
    this->speed = speed;

	if (!isSeq)
		animSeqLen = 0;
	setAnimationFunc(animation);
    animStartTime = millis();
}


void AlaLed::setAnimation(AlaSeq animSeq[])
{
    this->animSeq = animSeq;

    // initialize animSeqDuration and animSeqLen variables
    animSeqDuration = 0;
    for (animSeqLen = 0; animSeq[animSeqLen].animation != ALA_ENDSEQ; animSeqLen++)
    {
        animSeqDuration = animSeqDuration + animSeq[animSeqLen].duration;
    }
    animSeqStartTime = millis();
    this->duration = animSeq[0].duration;
    if (this->duration < refreshMillis) this->duration = refreshMillis;
    setAnimation(animSeq[0].animation, animSeq[0].speed, true);
}

void AlaLed::setSpeed(uint32_t speed)
{
    this->speed = speed;
	animStartTime = millis();
}


int AlaLed::getAnimation()
{
    return animation;
}


bool AlaLed::runAnimation()
{
    if (animation == ALA_STOPSEQ)
        return true;

    // skip the refresh if not enough time has passed since last update
    uint32_t cTime = millis();
    if (cTime < lastRefreshTime + refreshMillis)
        return false;

    // calculate real refresh rate
    refreshRate = 1000/(cTime - lastRefreshTime);

    lastRefreshTime = cTime;

    // run the animantion calculation
    if (animFunc != NULL)
        (this->*animFunc)();

    for (uint8_t x = 0; x < numLeds; x++)
    {
        if (leds[x] > MAX_PWM_VAL) leds[x] = MAX_PWM_VAL;
        if (leds[x] < MIN_PWM_VAL) leds[x] = MIN_PWM_VAL;
    }

    #ifdef DEBUGPRINTS
    Serial.print(animation_step);
    Serial.print(" led val-> ");
    Serial.println(leds[0]);
    #endif

    // update leds
    if (driver == ALA_PWM)
    {
        for (uint8_t i = 0; i < numLeds; i++)
            analogWrite(pins[i], leds[i]);
    }
    if (driver == ALA_PWM_INVERTED) 
    {
        for (uint8_t i = 0; i < numLeds; i++)
            analogWrite(pins[i], 255 - leds[i]);
    }
//    if (driver == ALA_TLC5940)
//    {
//       for(int i=0; i<numLeds; i++)
//           Tlc.set(pins[i], leds[i]*16);
//
//       Tlc.update();
//    }


    // if it's a sequence we have to calculate the current animation
    if (animSeqLen != 0)
    {
        uint32_t c = 0;
        uint32_t t = (cTime - animSeqStartTime) % animSeqDuration;
        for (uint16_t i = 0; i < animSeqLen; i++)
        {
            if (t >= c && t < (c + animSeq[i].duration))
            {
                setAnimation(animSeq[i].animation, animSeq[i].speed, true);
                this->duration = animSeq[i].duration;
                if (this->duration < refreshMillis) this->duration = refreshMillis;
                break;
            }
            c = c + animSeq[i].duration;
        }
    }

    return true;
}



///////////////////////////////////////////////////////////////////////////////


void AlaLed::setAnimationFunc(uint8_t animation)
{
    switch(animation)
    {
        case ALA_ON:                    animFunc = &AlaLed::on;                    break;
        case ALA_OFF:                   animFunc = &AlaLed::off;                   break;
        case ALA_BLINK:                 animFunc = &AlaLed::blink;                 break;
        case ALA_BLINKALT:              animFunc = &AlaLed::blinkAlt;              break;
        case ALA_SPARKLE:               animFunc = &AlaLed::sparkle;               break;
        case ALA_SPARKLE2:              animFunc = &AlaLed::sparkle2;              break;
        case ALA_STROBO:                animFunc = &AlaLed::strobo;                break;

        case ALA_PIXELSHIFTRIGHT:       animFunc = &AlaLed::pixelShiftRight;       break;
        case ALA_PIXELSHIFTLEFT:        animFunc = &AlaLed::pixelShiftLeft;        break;
        case ALA_PIXELBOUNCE:           animFunc = &AlaLed::pixelBounce;           break;
        case ALA_PIXELSMOOTHSHIFTRIGHT: animFunc = &AlaLed::pixelSmoothShiftRight; break;
        case ALA_PIXELSMOOTHSHIFTLEFT:  animFunc = &AlaLed::pixelSmoothShiftLeft;  break;
        case ALA_PIXELSMOOTHBOUNCE:     animFunc = &AlaLed::pixelSmoothBounce;     break;
        case ALA_COMET:                 animFunc = &AlaLed::comet;                 break;
        case ALA_BARSHIFTRIGHT:         animFunc = &AlaLed::barShiftRight;         break;
        case ALA_BARSHIFTLEFT:          animFunc = &AlaLed::barShiftLeft;          break;
        case ALA_LARSONSCANNER:         animFunc = &AlaLed::larsonScanner;         break;
        case ALA_LARSONSCANNER2:        animFunc = &AlaLed::larsonScanner2;        break;

        case ALA_FADEIN:                animFunc = &AlaLed::fadeIn;                break;
        case ALA_FADEOUT:               animFunc = &AlaLed::fadeOut;               break;
        case ALA_FADEINOUT:             animFunc = &AlaLed::fadeInOut;             break;
        case ALA_GLOW:                  animFunc = &AlaLed::glow;                  break;
        case ALA_FLAME:                 animFunc = &AlaLed::flame;                 break;

        case ALA_FADETO:                animFunc = &AlaLed::fadeTo;                break;

        case ALA_SET_LEVEL0:            animFunc = &AlaLed::setLevel0;             break;
        case ALA_SET_LEVEL1:            animFunc = &AlaLed::setLevel1;             break;
        case ALA_SET_LEVEL2:            animFunc = &AlaLed::setLevel2;             break;
        case ALA_SET_LEVEL3:            animFunc = &AlaLed::setLevel3;             break;

        case ALA_STOPSEQ:               animFunc = &AlaLed::skip;                  break;

        default:                        animFunc = &AlaLed::off;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////
// Switching effects
////////////////////////////////////////////////////////////////////////////////////////////

void AlaLed::on()
{
    for(int i=0; i<numLeds; i++)
    {
        leds[i] = maxOut;
    }
}

void AlaLed::off()
{
    for(int i=0; i<numLeds; i++)
    {
        leds[i] = 0;
    }
}

void AlaLed::skip()
{
    return;
}


void AlaLed::blink()
{
    int t = getStep(animStartTime, speed, 2);

    for(int x=0; x<numLeds; x++)
    {
        leds[x] = ((t+1)%2)*maxOut;
    }
}


void AlaLed::blinkAlt()
{
    int t = getStep(animStartTime, speed, 2);

    for(int x=0; x<numLeds; x++)
    {
        leds[x] = ((t+x)%2)*maxOut;
    }
}


void AlaLed::sparkle()
{
    int p = speed/100;
    for(int x=0; x<numLeds; x++)
    {
        leds[x] = (random(p)==0)*maxOut;
    }
}

void AlaLed::sparkle2()
{
    int p = speed/10;
    for(int x=0; x<numLeds; x++)
    {
        if (random(p)==0)
            leds[x] = maxOut;
        else
            leds[x] = leds[x] * 0.88;
    }
}



void AlaLed::strobo()
{
    int t = getStep(animStartTime, speed, ALA_STROBODC);

    for(int x=0; x<numLeds; x++)
    {
        leds[x] = (t==0)*maxOut;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////
// Shifting effects
////////////////////////////////////////////////////////////////////////////////////////////

void AlaLed::pixelShiftRight()
{
    int t = getStep(animStartTime, speed, numLeds);

    for(int x=0; x<numLeds; x++)
    {
        leds[x] = (x==t ? 1:0)*maxOut;
    }
}


void AlaLed::pixelShiftLeft()
{
    int t = getStep(animStartTime, speed, numLeds);

    for(int x=0; x<numLeds; x++)
    {
        leds[x] = (x==(numLeds-1-t) ? 1:0)*maxOut;
    }
}


void AlaLed::pixelBounce()
{
    int t = getStep(animStartTime, speed, 2*numLeds-2);

    for(int x=0; x<numLeds; x++)
    {
        leds[x] = (x==(-abs(t-numLeds+1)+numLeds-1) ? 1:0)*maxOut;
    }
}



void AlaLed::pixelSmoothShiftRight()
{
    float t = getStepFloat(animStartTime, speed, numLeds+1);

    for(int x=0; x<numLeds; x++)
    {
        leds[x] = max(0, (int)((-abs(t-1-x)+1)*maxOut));
    }
}


void AlaLed::pixelSmoothShiftLeft()
{
    float t = getStepFloat(animStartTime, speed, numLeds+1);

    for(int x=0; x<numLeds; x++)
    {
        leds[x] = max(0, (int)((-abs(numLeds-t-x)+1)*maxOut));
    }
}


void AlaLed::comet()
{
    float l = numLeds/2;  // length of the tail

    float t = getStepFloat(animStartTime, speed, 2*numLeds-l);

    for(int x=0; x<numLeds; x++)
    {
        leds[x] = (int)constrain( (((x-t)/l+1.2f))*(((x-t)<0)? 1:0)*maxOut, 0, maxOut);
    }
}


void AlaLed::barShiftRight()
{
    int l = numLeds/2+1;  // length of the bar

    int s = getStep(animStartTime, speed, numLeds+l);

    for(int x=0; x<numLeds; x++)
    {
        if(x<s && x>s-l)
            leds[x] = maxOut;
        else
            leds[x] = 0;
    }
}

void AlaLed::barShiftLeft()
{
    int l = numLeds/2+1;  // length of the bar

    int s = map(getStep(animStartTime, speed, 1000), 0, 1000, 0, numLeds+l);

    for(int x=0; x<numLeds; x++)
    {
        if(x<s && x>s-l)
            leds[numLeds-x-1] = maxOut;
        else
            leds[numLeds-x-1] = 0;
    }
}

void AlaLed::pixelSmoothBounce()
{
    // see larsonScanner
    float t = getStepFloat(animStartTime, speed, 2*numLeds-2);

    for(int x=0; x<numLeds; x++)
    {
        leds[x] = constrain((int)((-abs(abs(t-numLeds+1)-x)+1)*maxOut), 0, maxOut);
    }
}


void AlaLed::larsonScanner()
{
    float l = numLeds/4;
    float t = getStepFloat(animStartTime, speed, 2*numLeds-2);

    for(int x=0; x<numLeds; x++)
    {
        leds[x] = constrain((int)((-abs(abs(t-numLeds+1)-x)+l)*maxOut), 0, maxOut);
    }
}

void AlaLed::larsonScanner2()
{
    float l = numLeds/4;  // 2>7, 3-11, 4-14
    float t = getStepFloat(animStartTime, speed, 2*numLeds+(l*4-1));

    for(int x=0; x<numLeds; x++)
    {

        leds[x] = constrain((int)((-abs(abs(t-numLeds-2*l)-x-l)+l)*maxOut), 0, maxOut);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////
// Fading effects
////////////////////////////////////////////////////////////////////////////////////////////

void AlaLed::fadeIn()
{
    int16_t s = getStep(animStartTime, speed, maxOut);
    for (uint8_t x=0; x<numLeds; x++)
    {
        leds[x] = s;
    }
}


void AlaLed::fadeOut()
{
    int16_t s = getStep(animStartTime, speed, maxOut);
    for (uint8_t x=0; x<numLeds; x++)
    {
        leds[x] = maxOut - s;
    }
}


void AlaLed::fadeInOut()
{
    int16_t s = getStep(animStartTime, speed, 2*maxOut) - maxOut;

    for (uint8_t x=0; x<numLeds; x++)
    {
        // leds[x] = abs(maxOut-abs(s));
        leds[x] = maxOut - abs(s);
    }
}


void AlaLed::glow()
{
    float s = getStepFloat(animStartTime, speed, TWO_PI);

    for(int x=0; x<numLeds; x++)
    {
        leds[x] = (-cos(s)+1)*maxOut/2;
    }
}

void AlaLed::flame()
{
    for(int x=0; x<numLeds; x++)
    {
        if (random(4) == 0)
            leds[x] = map(random(30)+70, 0, 100, 0, maxOut);
    }
}

// FADETO LEVEL_N DURATION
void AlaLed::fadeTo()
{   //   return ( (millis()-animStartTime) % duration ) * v / duration; 
    uint8_t bln = speed;
    static uint8_t current_level = 0;

    if (bln >= MAX_B_LEVELS) bln = MAX_B_LEVELS - 1; // limit number of predefined leveles

    if (this->animation_step < 2)
    {
        current_level = leds[0];
    }

    int16_t s = 0;
    if (britness_level[bln] > current_level) {
        s = current_level + getStep(animStartTime, duration, britness_level[bln] - current_level);
    } else {
        s = current_level - getStep(animStartTime, duration, current_level - britness_level[bln]);
    }

    for (uint8_t x = 0; x < numLeds; x++)
    {
        leds[x] = s;
    }
}


void AlaLed::setLevel0()
{
    britness_level[0] = speed;
}

void AlaLed::setLevel1()
{
    britness_level[1] = speed;
}

void AlaLed::setLevel2()
{
    britness_level[2] = speed;
}

void AlaLed::setLevel3()
{
    britness_level[3] = speed;
}
