#include "stdafx.h"
#include "FirFilter.h"

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

FirFilter::FirFilter(const float32* coef, uint32 coeflen)
{
	if (coef == nullptr || coeflen == 0) {
		throw "Filter ctor failed";
	}
	len = coeflen * 2 - 1;
	h = new float32[len];	// 滤波器长度
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
	DWORD datalen = _wav.dataSize / sizeof(float32);

	if (_wav.pData == nullptr || datalen <= _start *  _wav.channels || _len == 0) {
		printf("wav object No data!\n");
		return;
	}
	DWORD leftlen = datalen - _start * _wav.channels;
	DWORD lentotal = ((DWORD)_len) * _wav.channels;// 待处理采样总个数
	lentotal = lentotal < leftlen ? lentotal : leftlen; 
	float32* pDataOffset = _wav.pData + _start * _wav.channels;	// 数据源地址
	float32* pProc = new float32[lentotal]();	// 初始化0

	int start, end;
	int i,j;
	if (_wav.GetChannels() == 1) {
		for (i = 0; i < lentotal; ++i) {
			start = - min(len / 2, i);
			end = min(lentotal - i - 1, len / 2);
			for (j = start; j <= end; ++j){ // 系数的下标
				pProc[i] += fderef(j) * pDataOffset[i + j];
			}
		}
	}
	else {
		stereo_f32_t pDualData = (stereo_f32_t)pDataOffset;
		stereo_f32_t pDualProc = (stereo_f32_t)pProc;
		int singlelen = lentotal / 2;
		for (i = 0; i < singlelen; ++i) {
			start = -min(len / 2, i);
			end = min(singlelen - i - 1, len / 2);
			for (j = start; j <= end; ++j){ // 系数的下标
				pDualProc[i][0] += fderef(j) * pDualData[i + j][0];
				pDualProc[i][1] += fderef(j) * pDualData[i + j][1];
			}
		}
	}

	memcpy_s(pDataOffset, lentotal * sizeof(float32), pProc, lentotal * sizeof(float32));
	delete[] pProc;
	pProc = nullptr;

}
