#pragma once

#include "WaveFile.h"

#define	fderef(index)	(*((this->flt) + (index))) // �����ã�Ϊ��֧�ָ����±�

class FirFilter
{
public:
	FirFilter(const float32* coef, uint32 coeflen);
	virtual ~FirFilter();
	virtual void ProcessWav(WaveFile& _wav, uint32 _start = 0, int _len = 0x7fffffff);

protected:
	float32* h;		// �����ڴ�
	float32* flt;	// ָ���˲�������
	uint32 len;
};

