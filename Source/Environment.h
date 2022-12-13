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
    Environment(int rate): tickRate(rate) {}

    const int tickRate;
};