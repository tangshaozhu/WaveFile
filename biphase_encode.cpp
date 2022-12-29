#include "biphase_encode.h"

#define START_BIT_VALUE 0
#define STOP_BIT_VALUE 1

#define PREAMBLE_LENGTH 12
#define CODE_LENGTH (1 + 8 + 1 + 1)
#define TAIL_LENGTH 3
#define KEEP_LEVEL 2
#define FIRST_PART 0
#define SECOND_PART 1

static uint16 _GetBiPhaseCode(uint16 bit_value, uint16 first_second_half, uint16 prev_level)
{
    switch (bit_value)
    {
    case 0:
        return !prev_level;
    case 1:
        return !prev_level ^ first_second_half;
    case KEEP_LEVEL:
        return prev_level;
    }
    return prev_level;
}

static uint16 _GetParityBit(BYTE n)
{
    uint16 ret = 1;
    while (n)
    {
        ret ^= 1;
        n &= n - 1;
    }
    return ret;
}



static uint16 _GetBitValueInByte(const BYTE* stream, size_t size, size_t rel_index)
{
    size_t quot = rel_index / CODE_LENGTH;
    size_t rem = rel_index % CODE_LENGTH;
    // start-bit
    if (rem == 0) {
        return START_BIT_VALUE;
    }
    // parity-bit
    if (rem == 9) {
        return _GetParityBit(stream[quot]);
    }
    // stop-bit
    if (rem == 10) {
        return STOP_BIT_VALUE;
    }
    // 1 ~ 8
	return !!(stream[quot] & (1 << (rem - 1)));
}

static uint16 _GetBitValue(const BYTE* stream, size_t size, size_t bit_index)
{
    // preamble
    if (bit_index < PREAMBLE_LENGTH) {
        return 1;
    }
    size_t rel_index = bit_index - PREAMBLE_LENGTH;
    // Header / Message / Checksum
    if (rel_index < CODE_LENGTH * size) {
        return _GetBitValueInByte(stream, size, rel_index);
    }
    // Tail
    return KEEP_LEVEL;
}


BYTE Checksum8(const BYTE* pdata, size_t size)
{
    BYTE checksum = 0;
    while (size--) {
        checksum ^= *pdata++;
    }
    return checksum;
}

sample_t* AllocSamples(const BYTE* stream, size_t size, uint32 sr, size_t* psample_len)
{
    #define SAMPLE_LEVEL 0.5
    // 12 bit preamble
    // 1 byte Header (11 bits)
    // n bytes Message (11 bits)
    // 1 Byte Checksum (11 bits)
    // 3 bit tail (turn off Modulator)
    // 2k Clock
    size_t sample_len = (12 + 3 + CODE_LENGTH * size) * sr / 2000;
    *psample_len = sample_len;
    sample_t* psample = new sample_t[sample_len];
	uint16 prev_level = 1;  // 前一个码字末尾电平
    uint16 curr_level;      // 当前末尾电平
	uint16 prev_bit_value = 0;
	uint16 bit_value;
	size_t prev_bit_index = 0xff;

    for (size_t i = 0; i < sample_len; i++)
    {
		size_t half_bit_index = i * 2000 * 2 / sr;
		size_t bit_index = half_bit_index / 2;

        if (bit_index != prev_bit_index) {
            bit_value = _GetBitValue(stream, size, bit_index);
			prev_level = !(prev_bit_value) ^ prev_level;
			prev_bit_value = bit_value;
            printf("%d, %d\n", bit_value, i);
        }
        psample[i] = SAMPLE_LEVEL * _GetBiPhaseCode(bit_value, half_bit_index & 1, prev_level);
        prev_bit_index = bit_index;
    }
	return psample;
}
