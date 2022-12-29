#pragma once
#include "WaveFile.h"

sample_t* AllocSamples(const BYTE* stream, size_t size, uint32 sr, size_t* psample_len);
BYTE Checksum8(const BYTE* pdata, size_t size);
