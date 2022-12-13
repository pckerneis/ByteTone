/*
  ==============================================================================

    ByteBeat.h
    Created: 13 Dec 2022 10:21:38pm
    Author:  pcker

  ==============================================================================
*/

#pragma once

class ByteBeat
{
public:
    static float integerToSample(int integer)
    {
        return (float)(integer & 255) / 127.5f - 1.0f;
    }
};