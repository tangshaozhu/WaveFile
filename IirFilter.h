#pragma once
#include "WaveFile.h"
#include "iir.h"

class IirFilter
{
public:
	IirFilter(mult_biquad_state& _sec1, mult_biquad_state& _sec2);
	virtual ~IirFilter();
	virtual void ProcessWav(WaveFile& _wav, uint32 _start = 0, int _len = 0x7fffffff);

protected:
	mult_biquad_state sec1L, sec2L, sec1R, sec2R;
};
