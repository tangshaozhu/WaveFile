#pragma once

#include "WaveFile.h"

#define	fderef(index)	(*((this->flt) + (index))) // 解引用，为了支持负数下标

class FirFilter
{
public:
	FirFilter(const float32* coef, uint32 coeflen);
	virtual ~FirFilter();
	virtual void ProcessWav(WaveFile& _wav, uint32 _start = 0, int _len = 0x7fffffff);

protected:
	float32* h;		// 申请内存
	float32* flt;	// 指向滤波器中心
	uint32 len;
};

