#pragma once
#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
	double gain;
	double a1, a2, a3, b1, b2, b3;
	double x1, x2, y1, y2;
}mult_biquad_state;

extern double mult_biquad(mult_biquad_state *state, double data);

#ifdef __cplusplus
}
#endif
