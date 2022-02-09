#include "stdafx.h"
#include "IirFilter.h"


IirFilter::IirFilter(mult_biquad_state& _sec1, mult_biquad_state& _sec2)
{
	sec1L = _sec1;
	sec1R = _sec1;
	sec2L = _sec2;
	sec2R = _sec2;
}


IirFilter::~IirFilter()
{
}


void IirFilter::ProcessWav(WaveFile& _wav, uint32 _start, int _len)
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

	int i;

	sec1L.x1 = 0.f;
	sec1L.x2 = 0.f;
	sec1L.y1 = 0.f;
	sec1L.y2 = 0.f;
	sec1R.x1 = 0.f;
	sec1R.x2 = 0.f;
	sec1R.y1 = 0.f;
	sec1R.y2 = 0.f;
	sec2L.x1 = 0.f;
	sec2L.x2 = 0.f;
	sec2L.y1 = 0.f;
	sec2L.y2 = 0.f;
	sec2R.x1 = 0.f;
	sec2R.x2 = 0.f;
	sec2R.y1 = 0.f;
	sec2R.y2 = 0.f;

	if (_wav.GetChannels() == 1) {
		for (i = 0; i < (int)lentotal; ++i) {
			pProc[i] = mult_biquad(&sec1L, pDataOffset[i]);
			pProc[i] = mult_biquad(&sec2L, pProc[i]);
		}
	}
	else {
		stereo_f32_t pDualData = (stereo_f32_t)pDataOffset;
		stereo_f32_t pDualProc = (stereo_f32_t)pProc;
		int singlelen = lentotal / 2;
		for (i = 0; i < singlelen; ++i) {
			pDualProc[i][0] = mult_biquad(&sec1L, pDualData[i][0]);
			pDualProc[i][0] = mult_biquad(&sec2L, pDualProc[i][0]);

			pDualProc[i][1] = mult_biquad(&sec1R, pDualData[i][1]);
			pDualProc[i][1] = mult_biquad(&sec2R, pDualProc[i][1]);		
		}
	}

	memcpy_s(pDataOffset, lentotal * sizeof(float32), pProc, lentotal * sizeof(float32));
	delete[] pProc;
	pProc = nullptr;
}