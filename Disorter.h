#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include "WaveFile.h"

class Disorter
{
public:
	Disorter();
	virtual ~Disorter();
	void ProcessWav(WaveFile& _wav, uint32 _start = 0, int _len = 0x7fffffff);

protected:
	virtual double GetSample(double input) = 0; // ´¿Ðéº¯Êý
};

