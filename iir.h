#pragma once
#ifdef __cplusplus
extern "C" {
#endif


typedef float sample_t;

typedef struct
{
	sample_t gain;
	sample_t a1, a2, a3, b1, b2, b3;
	sample_t x1, x2, y1, y2;
}mult_biquad_state;

extern sample_t mult_biquad(mult_biquad_state *state, sample_t data);

#ifdef __cplusplus
}
#endif
