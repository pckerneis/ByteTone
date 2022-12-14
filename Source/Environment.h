/*
  ==============================================================================

    Environment.h
    Created: 13 Dec 2022 10:48:26pm
    Author:  pcker

  ==============================================================================
*/

#pragma once

struct Environment
{
    static Environment withTickRate(int rate)
    {
        Environment env(rate);
        return env;
    }

    Environment(const Environment& other): tickRate(other.tickRate) { }

    int tickRate;

private:
    Environment(int rate) : tickRate(rate) {}
};