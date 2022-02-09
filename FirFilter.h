#pragma once

#include "WaveFile.h"

#define	fderef(index)	(*((this->flt) + (index))) // �����ã�Ϊ��֧�ָ����±�

class FirFilter
{
public:
	FirFilter(const double* coef, uint32 coeflen);
	virtual ~FirFilter();
	virtual void ProcessWav(WaveFile& _wav, uint32 _start = 0, int _len = 0x7fffffff);

protected:
	double* h;		// �����ڴ�
	double* flt;	// ָ���˲�������
	uint32 len;
};

