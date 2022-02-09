#include "stdafx.h"
#include "Disorter.h"


Disorter::Disorter()
{
}


Disorter::~Disorter()
{
}

void Disorter::ProcessWav(WaveFile& _wav, uint32 _start, int _len)
{
	DWORD datalen = _wav.dataSize / sizeof(float32);

	if (_wav.pData == nullptr || datalen <= _start *  _wav.channels || _len == 0) {
		printf("wav object No data!\n");
		return;
	}
	DWORD leftlen = datalen - _start * _wav.channels;
	DWORD lentotal = ((DWORD)_len) * _wav.channels;
	lentotal = lentotal < leftlen ? lentotal : leftlen;
	float32* pDataOffset = _wav.pData + _start * _wav.channels;	// ����Դ��ַ
	
	for (DWORD i = 0; i < lentotal; i++)
	{
		pDataOffset[i] = GetSample(pDataOffset[i]);
	}

}
