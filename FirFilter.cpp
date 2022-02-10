#include "stdafx.h"
#include "FirFilter.h"

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

FirFilter::FirFilter(const double* coef, uint32 coeflen)
{
	if (coef == nullptr || coeflen == 0) {
		throw "Filter ctor failed";
	}
	len = coeflen * 2 - 1;
	h = new double[len];	// 滤波器长度
	flt = h + coeflen - 1;
	for (int32 i = 0; i < (int32)coeflen; i++)
	{
		fderef(i) = coef[i];
		fderef(-i) = fderef(i);
	}
}


FirFilter::~FirFilter()
{
	if (h != nullptr) {
		delete[] h;
		h = nullptr;
	}
}

// 滤波处理。滤波器从中心点开始计算
void FirFilter::ProcessWav(WaveFile& _wav, uint32 _start, int _len)
{
	DWORD datalen = _wav.dataSize / sizeof(sample_t);

	if (_wav.pData == nullptr || datalen <= _start *  _wav.channels || _len == 0) {
		printf("wav object No data!\n");
		return;
	}
	DWORD leftlen = datalen - _start * _wav.channels;
	DWORD lentotal = ((DWORD)_len) * _wav.channels;// 待处理采样总个数
	lentotal = lentotal < leftlen ? lentotal : leftlen; 
	sample_t* pDataOffset = _wav.pData + _start * _wav.channels;	// 数据源地址
	sample_t* pProc = new sample_t[lentotal]();	// 初始化0

	int start, end;
	int i,j;
	if (_wav.GetChannels() == 1) {
		for (i = 0; i < lentotal; ++i) {
			start = -(int)min(len / 2, i);
			end = min(lentotal - i - 1, len / 2);
			for (j = start; j <= end; ++j){ // 系数的下标
				pProc[i] += fderef(j) * pDataOffset[i + j];
			}
		}
	}
	else {
		stereo_t pDualData = (stereo_t)pDataOffset;
		stereo_t pDualProc = (stereo_t)pProc;
		int singlelen = lentotal / 2;
		for (i = 0; i < singlelen; ++i) {
			start = -(int)min(len / 2, i);
			end = min(singlelen - i - 1, len / 2);
			for (j = start; j <= end; ++j){ // 系数的下标
				pDualProc[i][0] += fderef(j) * pDualData[i + j][0];
				pDualProc[i][1] += fderef(j) * pDualData[i + j][1];
			}
		}
	}

	memcpy_s(pDataOffset, lentotal * sizeof(sample_t), pProc, lentotal * sizeof(sample_t));
	delete[] pProc;
	pProc = nullptr;

}
