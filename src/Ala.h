/*--------------------------------------------------------------------
This file is part of the Arduino ALA library.

The Arduino ALA library is free software: you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

The Arduino ALA library is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with The Arduino ALA library.  If not, see
<http://www.gnu.org/licenses/>.
--------------------------------------------------------------------*/

#ifndef Ala_h
#define Ala_h

#include <stdint.h>
#include "Arduino.h"


////////////////////////////////////////////////////////////////////////////////
// Drivers

#define ALA_PWM 1
#define ALA_PWM_INVERTED 2
#define ALA_TLC5940 3
#define ALA_WS2812 4


////////////////////////////////////////////////////////////////////////////////
// Animations

#define ALA_ON 2
#define ALA_OFF 3
#define ALA_BLINK 4
#define ALA_BLINKALT 5
#define ALA_SPARKLE 6
#define ALA_SPARKLE2 7
#define ALA_STROBO 8

#define ALA_CYCLECOLORS 9

#define ALA_PIXELSHIFTRIGHT 10
#define ALA_PIXELSHIFTLEFT 11
#define ALA_PIXELBOUNCE 12
#define ALA_PIXELSMOOTHSHIFTRIGHT 13
#define ALA_PIXELSMOOTHSHIFTLEFT 14
#define ALA_PIXELSMOOTHBOUNCE 15
#define ALA_COMET 16
#define ALA_COMETCOL 17
#define ALA_BARSHIFTRIGHT 18
#define ALA_BARSHIFTLEFT 19
#define ALA_MOVINGBARS 20
#define ALA_MOVINGGRADIENT 21
#define ALA_LARSONSCANNER 22
#define ALA_LARSONSCANNER2 23

#define ALA_FADEIN 24
#define ALA_FADEOUT 25
#define ALA_FADEINOUT 26
#define ALA_GLOW 27
#define ALA_PLASMA 28

#define ALA_FADECOLORS 29
#define ALA_FADECOLORSLOOP 30
#define ALA_PIXELSFADECOLORS 31
#define ALA_FLAME 32

#define ALA_FIRE 33
#define ALA_BOUNCINGBALLS 34
#define ALA_BUBBLES 35

#define ALA_FADETO      37

#define ALA_SET_LEVEL0  38
#define ALA_SET_LEVEL1  39
#define ALA_SET_LEVEL2  40
#define ALA_SET_LEVEL3  41

#define ALA_ENDSEQ 0
#define ALA_STOPSEQ 1

////////////////////////////////////////////////////////////////////////////////

// Strobo effect duty cycle (10=1/10)
#define ALA_STROBODC 36


////////////////////////////////////////////////////////////////////////////////

#pragma pack(push, 1)

struct AlaColor
{
    union
    {
        struct
        {
            uint8_t r;
            uint8_t g;
            uint8_t b;
        };
        uint8_t raw[3];
    };

    inline AlaColor() __attribute__((always_inline))
    {
    }

    // allow construction from R, G, B
    inline AlaColor( uint8_t ir, uint8_t ig, uint8_t ib)  __attribute__((always_inline))
    : r(ir), g(ig), b(ib)
    {
    }

    // allow construction from 32-bit (really 24-bit) bit 0xRRGGBB color code
    inline AlaColor( uint32_t colorcode)  __attribute__((always_inline))
    : r((colorcode >> 16) & 0xFF), g((colorcode >> 8) & 0xFF), b((colorcode >> 0) & 0xFF)
    {
    }

    bool operator == (const AlaColor &c) const
    {
        return(this->r == c.r and this->g == c.g and this->b == c.b);
    }
/*
    AlaColor getPixel(AlaColor maxOut)
    {
        return AlaColor(r*maxOut.r/255, g*maxOut.g/255, b*maxOut.b/255);
    }
*/
    AlaColor sum(AlaColor color)
    {
        int r0 = _min(color.r + r, 255);
        int g0 = _min(color.g + g, 255);
        int b0 = _min(color.b + b, 255);
        return AlaColor(r0, g0, b0);
    }

    AlaColor interpolate(AlaColor color, float x)
    {
        int r0 = x*(color.r - r) + r;
        int g0 = x*(color.g - g) + g;
        int b0 = x*(color.b - b) + b;
        return AlaColor(r0, g0, b0);
    }

    AlaColor scale(float k)
    {
        int r0 = _min(r*k, 255);
        int g0 = _min(g*k, 255);
        int b0 = _min(b*k, 255);
        return AlaColor(r0, g0, b0);
    }


    typedef enum {
        Aqua    = 0x00FFFF,
        Black   = 0x000000,
        Blue    = 0x0000FF,
        Cyan    = 0x00FFFF,
        Gold    = 0xFFD700,
        Gray    = 0x808080,
        Green   = 0x008000,
        Lime    = 0x00FF00,
        Magenta = 0xFF00FF,
        Navy    = 0x000080,
        Olive   = 0x808000,
        Purple  = 0x800080,
        Red     = 0xFF0000,
        Teal    = 0x008080,
        White   = 0xFFFFFF,
        Yellow  = 0xFFFF00
    } ColorCodes;
} ;



////////////////////////////////////////////////////////////////////////////////
// Struct definitions
////////////////////////////////////////////////////////////////////////////////

struct AlaPalette
{
    uint16_t numColors;
    AlaColor *colors;

    /**
    * Get the interpolated color from the palette.
    * The argument is a floating number between 0 and numColors.
    */
    AlaColor getPalColor(float i)
    {
        int i0 = (int)i%(numColors);
        int i1 = (int)(i+1)%(numColors);

        // decimal part is used to interpolate between the two colors
        float t0 = i - trunc(i);
        //float t0 = i - (int)i;

        return colors[i0].interpolate(colors[i1], t0);
    }

    bool operator == (const AlaPalette &c) const
    {
        if (!(this->numColors == c.numColors))
            return false;

        for(int i=0; i<numColors; i++)
        {
            if (!(this->colors[i] == c.colors[i]))
                return false;
        }
        return true;
    }

};


//int ardsdasdrr = sizeof(AlaSeq);

#pragma pack(pop)

////////////////////////////////////////////////////////////////////////////////
// Palette definitions
////////////////////////////////////////////////////////////////////////////////

// Empty palette
extern AlaPalette alaPalNull;

// Red,Green,Blue sequence
extern AlaPalette alaPalRgb;

// Rainbow colors
extern AlaPalette alaPalRainbow;

// Rainbow colors with alternating stripes of black
extern AlaPalette alaPalRainbowStripe;

extern AlaPalette alaPalParty;

extern AlaPalette alaPalHeat;

// Fire palette to be used with ALA_FIRE effect
extern AlaPalette alaPalFire;

extern AlaPalette alaPalCool;




////////////////////////////////////////////////////////////////////////////////
// Utility functions
////////////////////////////////////////////////////////////////////////////////

int16_t getStep(long t0, long t, int v);
float getStepFloat(long t0, long t, float v);
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max);


#endif